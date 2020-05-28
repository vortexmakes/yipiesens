/***********************************************************************************************\
* Freescale MMA8451,2,3Q Driver
*
* Filename: terminal.c
*
*
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/

#include "system.h"
#include "serial.h"
#include "terminal.h"

/***********************************************************************************************\
* Private macros
\***********************************************************************************************/

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

byte ProcessHexInput (byte *in, byte *out);
void CopyXYZ (byte *ptr);
void CopyXYZ8 (byte *ptr);
void PrintXYZdec14 (void);
void PrintXYZdec12 (void);
void PrintXYZdec10(void);
void PrintXYZdec8 (void);
void PrintXYZfrac (void);
void ClearDataFlash (void);
void PrintBuffering (void);
void PrintFIFO (void);
void ReadDataFlashXYZ (void);
void WriteFlashBuffer (void);

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

BIT_FIELD StreamMode;                       // stream mode control flags

extern BIT_FIELD RegisterFlag;
extern byte value[];                        // working value result scratchpad

static byte temp;                          //  byte variables
static byte index;                          // input character string index

static tword x_value;                       // 16-bit X accelerometer value
static tword y_value;                       // 16-bit Y accelerometer value
static tword z_value;                       // 16-bit Z accelerometer value

extern byte SlaveAddressIIC;                // accelerometer slave I2C address

extern byte functional_block;               // accelerometer function
                                        
extern byte address_in[3];                  // Data Flash input address pointer
extern byte address_out[3];                 // Data Flash output address pointer

byte InputStringIndex;
#define BUFFER_RX_SIZE        10
#define BUFFER_TX_SIZE        200
byte BufferRx[BUFFER_RX_SIZE];

#pragma DATA_SEG DEFAULT

extern tfifo_sample fifo_data[FIFO_BUFFER_SIZE];   // FIFO sample buffer


byte *const ODRvalues [] = 
{
  "800", "400", "200", "100", "50", "12.5", "6.25", "1.563"

};

byte *const HPvaluesN [] =
{
  "16", "8", "4", "2",  //800
  "16", "8", "4", "2",  //400
  "8", "4", "2", "1",   //200
  "4", "2", "1", "0.5",   //100
  "2", "1", "0.5", "0.25", //50
  "2", "1", "0.5", "0.25", //12.5
  "2", "1", "0.5", "0.25", //6.25
  "2", "1", "0.5", "0.25"
};

byte *const HPvaluesLNLP [] =
{
  "16", "8", "4", "2",
  "16", "8", "4", "2",
  "8", "4", "2", "1",
  "4", "2", "1", "0.5",
  "2", "1", "0.5", "0.25",
  "0.5", "0.25", "0.125", "0.063",
  "0.25", "0.125", "0.063", "0.031",
  "0.063", "0.031", "0.016", "0.008"
};

byte *const HPvaluesHiRes [] =
{
  "16", "8", "4", "2"
};

byte *const HPvaluesLP [] =
{
 "16", "8", "4", "2",
  "8", "4", "2", "1",
  "4", "2", "1", "0.5",
  "2", "1", "0.5", "0.25",
  "1", "0.5", "0.25", "0.125",
  "0.25", "0.125", "0.063", "0.031",
  "0.125", "0.063", "0.031", "0.016",
  "0.031", "0.016", "0.008", "0.004" 
};

byte *const GRange [] =
{
  "2g", "4g", "8g"
};


/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
**  Terminal Strings
\*********************************************************/
const byte string_Prompt []       = {"\r\nMMA845xQ> "};
const byte string_What []         = {" <-- what?"};
const byte string_Streaming []    = {" - Streaming XYZ data"};
const byte string_Counts []       = {" as signed counts\r\n"};  
const byte string_Gs []           = {" as signed g's\r\n"};
const byte string_Interrupts []   = {" via Int Mode "};
const byte string_FIFO []         = {" via FIFO"};

/*********************************************************\
**  Initialize Terminal Interface
\*********************************************************/
void TerminalInit (void)
{
  
  SCISendString("\r\n\n**  Freescale Semiconductor  **");
  SCISendString  ("\r\n**  MMA845xQ Driver **"); 
  SCISendString  ("\r\n**      using the MC9S08QE8  **");
  SCISendString  ("\r\n**                           **");
  SCISendString  ("\r\n**  DATE TIME **\r\n\n");
  /*
  **  Prepare Data Flash.
  */
  DATAFLASH_Unprotect();
  address_in[0] = 0;
  address_in[1] = 0;
  address_in[2] = 0;
  address_out[0] = 0;
  address_out[1] = 0;
  address_out[2] = 0;

  SPI_SS_SELECT;
  SPI_ChrShift(0x01);
  SPI_ChrShift(0x00);
  SPI_SS_DESELECT;

  functional_block = FBID_MAX;
  StreamMode.Byte = 0;
  
}


/*********************************************************\
**  Process Terminal Interface
\*********************************************************/

void ProcessTerminal (void)
{
  /*
  **  Output command prompt if required.
  */
  if (PROMPT_WAIT == FALSE)
  {
  
    SCISendString((byte*)string_Prompt);
   
    PROMPT_WAIT = TRUE;
  }
  /*
  **  Get input from terminal.
  */
  if (SCI_INPUT_READY == TRUE)
  {
    INPUT_ERROR = FALSE;
    /*
    **  Use command line input if not streaming data
    */
    if ((XYZ_STREAM == FALSE) && (INT_STREAM == FALSE))
    {
      PROMPT_WAIT = FALSE;
      /*
      **  Get first input character - only uppercase
      */
      switch (BufferRx[0])
      {
        /***************************************************************************************/                 
        case '?':
          /*
          **  Help : list valid commands
          */              
          SCISendString("\r\nMn  : Mode  1= Standby; 2= 2g; 4= 4g; 8= 8g\r\n");
          SCISendString("On      : Oversampling 0=Normal 1=LNLP  2=HiRes 3=LP\r\n");
          SCISendString("ROn     : ODRHz 0=800; 1=400; 2=200; 3=100; 4=50; 5=12.5; 6=6.25; 7=1.56\r\n"); 
          SCISendString("RRxx    : Register xx Read\r\n");
          SCISendString("RWxx=nn : Register xx Write value nn\r\n");
          SCISendString("RHn     : High Pass Filter  0 - 3\r\n");
          SCISendString("RF      : Report ODR speed, HP Filter freq and Mode\r\n");
          /*SCISendString("Ca      : Data signed counts a=L LPF data a=H HPF data \r\n");
          //SCISendString("Ga      : XYZ data as signed g's a=L LPF data a=H HPF data \r\n");  */
          //SCISendString("Saa     : Stream Data: aa CL=CountsLPF,CH=CountsHPF,GL=G'sLPF,GH=G'sHPF\r\n");             
          
          SCISendString("Iaan    : Stream Data via Interrupts.\r\n");
          SCISendString("        : aa CL=CountsLPF, CH=CountsHPF,GL=G'sLPF,GH=G'sHPF INTn\r\n");
          SCISendString("        : n  1= INT1; 2= INT2\r\n");

          if (deviceID==1) 
          {
          SCISendString("Faaww   : Stream 14-bit XYZ via FIFO\r\n");
          SCISendString("        : aa CL=CountsLPF,CH=CountsHPF,GL=G'sLPF,GH=G'sHPF\r\n");         
          SCISendString("        : ww: Watermark= 1 to 31\r\n");
          }
          break;
          
        /***************************************************************************************/
        case 'M':
          /*
          **  Control sensor mode of operation
          */
          switch (BufferRx[1])
          {
            
            case '1':
              /*
              **  M1  : Enter Standby
              */
              SCISendString(" - Sensor Standby");
              MMA845x_Standby();
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case '2':
              /*
              **  M2  : Enter Active 2g
              */
              SCISendString(" - Sensor Active 2g");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~FS_MASK));
              IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | FULL_SCALE_2G));
              full_scale = FULL_SCALE_2G;              
              MMA845x_Active();
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case '4':
              /*
              **  M4  : Enter Active 4g
              */
              SCISendString(" - Sensor Active 4g");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~FS_MASK));
              IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | FULL_SCALE_4G));
              full_scale = FULL_SCALE_4G;
              MMA845x_Active();
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case '8':
              /*
              **  M8  : Enter Active 8g
              */
              SCISendString(" - Sensor Active 8g");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~FS_MASK));
              IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | FULL_SCALE_8G));
              full_scale = FULL_SCALE_8G;
              MMA845x_Active();
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            default:
              INPUT_ERROR = TRUE;
              break;
          }
          break;


          case 'O':
          /*
          **  Oversampling Mode Changes
          */
          
          switch (BufferRx[1])
          {
            
            case '0':
              /*
              **  0  : Oversampling Mode: Normal
              */
              SCISendString(" - Oversampling Mode set to Normal");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & ~MODS_MASK));
              MMA845x_Active();
              break;
              
            case '1':
              /*
              **  1  : Oversampling Mode: LOW NOISE LOW POWER
              */
              SCISendString(" - Oversampling Mode set to Low Noise Low Power");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & ~MODS_MASK));
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) | MODS0_MASK));
              MMA845x_Active();
              break;
              
            case '2':
              /*
              **  2  : Oversampling Mode: HI RESOLUTION
              */
              SCISendString(" - Oversampling Mode set to Hi Resolution");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & ~MODS_MASK));
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) | MODS1_MASK));
              MMA845x_Active();
              break;
              
            case '3':
              /*
              **  3  : Oversampling Mode: LOW POWER
              */
              SCISendString(" - Oversampling Mode set to Low Power");
              MMA845x_Standby();
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & ~MODS_MASK));
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (IIC_RegRead(SlaveAddressIIC, CTRL_REG2) | MODS_MASK));
              MMA845x_Active();
              break;
              
              default:
              INPUT_ERROR = TRUE;
              break;
          }
          break;
  
             
        /***************************************************************************************/
        //case 'C':
          /*
          
          **  Read and display full(14b/10b/8b) XYZ data as signed counts
          */
          //Make sure FMODE=00
          //IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, 0x00);

         //switch (BufferRx[1])
          //{
          //  case 'L':
              /*
              **  LPF Data
              */
           //   SCISendString(" - Low Pass Filtered Data");              
           //   MMA845x_Standby();
           //   IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
           //   MMA845x_Active();
           //   break;

            /////////////////////////////////////////////////////////////////////////////////////
           // case 'H':
              /*
              ** HPF Data
              */
          //    SCISendString(" - High Pass Filtered Data");
          //    MMA845x_Standby();
          //    IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
          //    MMA845x_Active();
          //    break;

          //  default:
          //    INPUT_ERROR = TRUE;
          //  break;
          //}
          
        //  IIC_RegReadN(SlaveAddressIIC, OUT_X_MSB_REG, 6, &value[0]);
        //  CopyXYZ(&value[0]);
        //  if (deviceID==1) 
        //  {
        //      PrintXYZdec14();
        //  } 
        //  else if (deviceID==2) 
         // {
         //      PrintXYZdec12();
         // } 
         // else if (deviceID==3) 
         // {
         //      PrintXYZdec10();
         // }           
                             
         // break;

        /***************************************************************************************/
        //case 'G':
          /*
          **  Read and display full 14-bit/10-bit/8-bit XYZ data as signed g's
          */
        //  IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, 0x00);

        //  switch (BufferRx[1])
        //  {
        //    case 'L':
              /*
              **  LPF Data
              */
        //      SCISendString(" - Low Pass Filtered Data");              
         //     MMA845x_Standby();
         //     IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
         //     MMA845x_Active();
         //     break;

            /////////////////////////////////////////////////////////////////////////////////////
          //  case 'H':
              /*
              ** HPF Data
              */
            //  SCISendString(" - High Pass Filtered Data");
            //  MMA845x_Standby();
            //  IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
            //  MMA845x_Active();
            //  break;

              //default:
              //INPUT_ERROR = TRUE;
              //break;
          //}
          
         // IIC_RegReadN(SlaveAddressIIC, OUT_X_MSB_REG, 6, &value[0]);
         // CopyXYZ(&value[0]);
         // PrintXYZfrac();         
               
        //  break;


        /***************************************************************************************/
        case 'R':
          /*
          **  Sensor register access
          */
          switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case 'R':
              /*
              **  RR xx  : Register Read
              */
              index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              /*
              **  Process hexadecimal register address input
              */
              temp = ProcessHexInput (&BufferRx[index], &value[0]);
              if (temp != 0)
              {
                index += temp;
                /*
                **  Check for end of string null
                */
                if (BufferRx[index++] == 0)
                {
                  SCISendString("= ");
                  /*
                  **  Go read register and report result
                  */
                  temp = IIC_RegRead(SlaveAddressIIC, value[0]);
                  hex2ASCII(temp,&value[0]);
                  value[2] = 0;
                  SCISendString(&value[0]);
                }
                else
                {
                  INPUT_ERROR = TRUE;
                }
              }
              else
              {
                /*
                **  If no register identified, then read all registers
                */
                SCISendString("Read All Registers\r\n");
                for (value[5]=0; value[5]<0x3F;)
                {
                  hex2ASCII(value[5],&value[0]);
                  value[2] = ' ';
                  value[3] = '=';
                  value[4] = 0;
                  SCISendString(&value[0]);

                  for (value[4]=4; value[4]!=0; value[4]--)
                  {
                    value[0] = ' ';
                    temp = IIC_RegRead(SlaveAddressIIC, value[5]++);
                    hex2ASCII(temp,&value[1]);
                    value[3] = 0;
                    SCISendString(&value[0]);
                  }
                  SCI_putCRLF();
                }
              }
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'W':
              /*
              **  RW xx = nn  : Register Write
              */
              index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              /*
              **  Process hexadecimal register address input
              */
              temp = ProcessHexInput (&BufferRx[index], &value[0]);
              if (temp != 0)
              {
                index += temp;
                /*
                **  Check for "="
                */
                while (BufferRx[index] == ' ')
                {
                  index++;
                }
                if (BufferRx[index++] == '=')
                {
                  while (BufferRx[index] == ' ')
                  {
                    index++;
                  }
                  /*
                  **  Process hexadecimal register data input
                  */
                  temp = ProcessHexInput (&BufferRx[index], &value[1]);
                  if (temp != 0)
                  {
                    index += temp;
                    /*
                    **  Check for end of string null
                    */
                    if (BufferRx[index++] == 0)
                    {
                      /*
                      **  Go write register
                      */
                      MMA845x_Standby();
                      IIC_RegWrite(SlaveAddressIIC, value[0], value[1]);
                      MMA845x_Active();
                      /*
                      **  Go read register and verify
                      */
                      temp = IIC_RegRead(SlaveAddressIIC, value[0]);
                      if (temp == value[1])
                      {
                        SCISendString(" Success");
                        /*
                        **  Check and flag if ODR was set to 400Hz or 800Hz
                        */
                        if (value[0] == CTRL_REG1)
                        {
                          if ( ((temp &DR_MASK) == 0) || ((temp &DR_MASK)==8) )   //Checking for 400Hz or 800Hz
                          {
                            ODR_400 = TRUE;
                          } 
                      
                          else
                          {
                            ODR_400 = FALSE;
                          }
                        }
                      }
                      else
                      {
                        SCISendString(" Fail");
                      }
                    }
                    else
                    {
                      INPUT_ERROR = TRUE;
                    }
                  }
                  else
                  {
                    INPUT_ERROR = TRUE;
                  }
                }
                else
                {
                  INPUT_ERROR = TRUE;
                }
              }
              else
              {
                INPUT_ERROR = TRUE;
              }
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'O':
              /*
              **  RO n  : Change Output Data Rate Register
              */
              index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              /*
              **  Accept numerical entries from '0' to '7'
              */
              value[0] = BufferRx[index] - '0';
              if (value[0] < 8)
              {
                value[0] <<= 3;    //shift the value over by 3 bits
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG1,IIC_RegRead(SlaveAddressIIC,CTRL_REG1) & ~DR_MASK);
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG1,IIC_RegRead(SlaveAddressIIC,CTRL_REG1) |value[0]);
                MMA845x_Active();
                /*
                **  Echo back both ODR and HP register values
                */
                
                
                Print_ODR_HP(); // Print Data Rate, Cut-off Frequency for HPF and g-range
              }
              else
              {
                INPUT_ERROR = TRUE;
              }
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'H':
              /*
              **  RH n  : Change High Pass Filter Register
              */
              index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              /*
              **  Accept numerical entries from '0' to '3'
              */
              value[0] = BufferRx[index] - '0';
              if (value[0] < 4)
              {
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, HP_FILTER_CUTOFF_REG,(IIC_RegRead(SlaveAddressIIC, HP_FILTER_CUTOFF_REG)& ~SEL_MASK));
                IIC_RegWrite(SlaveAddressIIC, HP_FILTER_CUTOFF_REG,(IIC_RegRead(SlaveAddressIIC, HP_FILTER_CUTOFF_REG)| value[0]));
                MMA845x_Active();
                /*
                **  Echo back both ODR and HP register values
                */
                Print_ODR_HP();
              }
              else
              {
                INPUT_ERROR = TRUE;
              }
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'F':
              /*
              **  RF  : Report ODR and HP Frequencies
              */
              Print_ODR_HP();
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            default:            
              INPUT_ERROR = TRUE;
              break;
          }
          break;

        /***************************************************************************************/
        //case 'S':
          /*
          **  Stream full XYZ data
          */
          //Make Sure FMODE=00
        // IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, 0x00);

         //switch (BufferRx[1])
         //{
            /////////////////////////////////////////////////////////////////////////////////////
         //   case 'C':
              /*
              **  SC  : Stream full resolution XYZ data as signed counts
              */
         //     SCISendString((byte*)string_Streaming);
         //     SCISendString((byte*)string_Counts);
              
         //     switch (BufferRx[2])
         //     {
         //      case 'L':
                /*
                **  LPF Data
                */
         //      SCISendString(" - Low Pass Filtered Data");              
         //      MMA845x_Standby();
         //      IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
         //      MMA845x_Active();
         //      break;
              /////////////////////////////////////////////////////////////////////////////////////
         //     case 'H':
                /*
                ** HPF Data
                */
         //       SCISendString(" - High Pass Filtered Data");
         //       MMA845x_Standby();
         //       IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
         //       MMA845x_Active();
         //       break;
              
         //     }
              
         //     StreamMode.Byte = STREAM_FULLC_MASK;
         //     functional_block = FBID_FULL_XYZ_SAMPLE;
         //     POLL_ACTIVE = TRUE;
         //     XYZ_STREAM = TRUE;
         //     PROMPT_WAIT = TRUE;                
         //     break;

            /////////////////////////////////////////////////////////////////////////////////////
         //   case 'G':
            
              /*
              **  SG  : Stream  XYZ data as signed g's
              */

         //     SCISendString((byte*)string_Streaming);
         //    SCISendString((byte*)string_Gs);
              
         //     switch (BufferRx[2])
         //     {
         //       case 'L':
               /*
                **  LPF Data
                */
         //       SCISendString(" - Low Pass Filtered Data");              
         //       MMA845x_Standby();
         //       IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
         //       MMA845x_Active();
         //       break;
             
              /////////////////////////////////////////////////////////////////////////////////////
         //   case 'H':
                /*
                ** HPF Data
                */
         //       SCISendString(" - High Pass Filtered Data");
         //       MMA845x_Standby();
         //       IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
         //       MMA845x_Active();
         //       break;

         //       default:
         //       INPUT_ERROR = TRUE;
         //       break;
         //     }   
         //     StreamMode.Byte = STREAM_FULLG_MASK;
         //     functional_block = FBID_FULL_XYZ_SAMPLE;
         //     POLL_ACTIVE = TRUE;
         //     XYZ_STREAM = TRUE;
         //     PROMPT_WAIT = TRUE;
              
         //     break;
         
            /////////////////////////////////////////////////////////////////////////////////////
         //     default:
         //     INPUT_ERROR = TRUE;
         //     break;
         // }
         // break; 

        /////////////////////////////////////////////////////////////////////////////////////////
        case 'I':
          /*
          **  Stream XYZ data via interrupts ICL n  ICH n IGL n IGH n (n=1 or 2)
          */
         IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, 0x00);

          index = 3;
          StreamMode.Byte = 0;
          /*
          **  Skip past space characters
          */
          while (BufferRx[index] == ' ')
            index++;

          INT_STREAM = TRUE;
          /*
          **  Determine selected output format
          */
          switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case 'C':
              /*
              **  IC a n : Stream XYZ data via INTn as signed counts
              */
              STREAM_FULLC = TRUE;
              
              switch (BufferRx[2])
              {
                case 'L':
                /*
                **  LPF Data
                */
                SCISendString(" - Low Pass Filtered Data");              
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
                MMA845x_Active();
                break;

              /////////////////////////////////////////////////////////////////////////////////////
                case 'H':
                /*
                ** HPF Data
                */
                SCISendString(" - High Pass Filtered Data");
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
                MMA845x_Active();
                break;

                default:
                  INPUT_ERROR = TRUE;
                break;
              }     
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'G':
              /*
              **  IG n : Stream  XYZ data via INTn as signed g's
              */
              STREAM_FULLG = TRUE;
              switch (BufferRx[2])
              {
                  case 'L':
                  /*
                  **  LPF Data
                  */
                  SCISendString(" - Low Pass Filtered Data");              
                  MMA845x_Standby();
                  IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
                  MMA845x_Active();
                  break;

                /////////////////////////////////////////////////////////////////////////////////////
                  case 'H':
                  /*
                  ** HPF Data
                  */
                  SCISendString(" - High Pass Filtered Data");
                  MMA845x_Standby();
                  IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
                  MMA845x_Active();
                  break;

                  default:
                  INPUT_ERROR = TRUE;
                  break;
              }    
              
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            default:
              INPUT_ERROR = TRUE;
              INT_STREAM = FALSE;
              break;
          }
          // End of Main Case Statement for "I"
          
          
          
          break;

        /***************************************************************************************/
        case 'F':
          /*
          **  Stream XYZ data via FIFO interrupts
          */
          index = 3;   // watermark value
          StreamMode.Byte = 0;
          /*
          **  Skip past space characters
          */
          while (BufferRx[index] == ' ')
            index++;

          /*
          **  Get the desired FIFO watermark.
          */
          temp = 0;
          if (isnum (BufferRx[index]) == TRUE)
          {
            temp = BufferRx[index++] - '0';
            if (isnum (BufferRx[index]) == TRUE)
            {
              temp = temp * 10;
              temp += (BufferRx[index++] - '0');
            }
          }

          INT_STREAM = TRUE;
          /*
          **  Determine selected output format
          */
          switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case 'C':
              /*
              **  FC ww : Stream 14-bit XYZ data via FIFO as signed counts using watermark ww
              */
              SCISendString((byte*)string_Streaming);
              SCISendString((byte*)string_FIFO);
              SCISendString((byte*)string_Counts);
              STREAM_FULLC = TRUE;
              switch (BufferRx[2])
              {
                case 'L':
                /*
                **  LPF Data
                */
                SCISendString(" - Low Pass Filtered Data");              
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
                MMA845x_Active();
                break;

              /////////////////////////////////////////////////////////////////////////////////////
                case 'H':
                /*
                ** HPF Data
                */
                SCISendString(" - High Pass Filtered Data");
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
                MMA845x_Active();
                break;
                
                default:
                  INPUT_ERROR = TRUE;
                break;
              }    
              
            break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'G':
              /*
              **  FG ww : Stream 14-bit XYZ data via FIFO as signed g's using watermark ww
              */
              SCISendString((byte*)string_Streaming);
              SCISendString((byte*)string_FIFO);
              SCISendString((byte*)string_Gs);
              STREAM_FULLG = TRUE;
              switch (BufferRx[2])
              {
                case 'L':
                /*
                **  LPF Data
                */
                SCISendString(" - Low Pass Filtered Data");              
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) & ~HPF_OUT_MASK));
                MMA845x_Active();
                break;

              /////////////////////////////////////////////////////////////////////////////////////
                case 'H':
                /*
                ** HPF Data
                */
                SCISendString(" - High Pass Filtered Data");
                MMA845x_Standby();
                IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, (IIC_RegRead(SlaveAddressIIC, XYZ_DATA_CFG_REG) | HPF_OUT_MASK)); 
                MMA845x_Active();
                break;

                default:
                 INPUT_ERROR = TRUE;
                break;
              }    
              
            break;
            /////////////////////////////////////////////////////////////////////////////////////
            default:
              INPUT_ERROR = TRUE;
              INT_STREAM = FALSE;
            break;
          }  // End of Case Statement for "F"
          
          if (INT_STREAM == TRUE)
          {
            /*
            **  Configure the FIFO
            **  - contains the most recent samples, discarding oldest when overflowed
            **  - set the watermark
            */
            if (temp > 31)
              temp = 0;
            MMA845x_Standby();
            IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, F_MODE0_MASK + temp);

            functional_block = FBID_FIFO;
            PROMPT_WAIT = TRUE;
            /*
            **  Configure the interrupt pins for falling edge and open drain output
            */

            IIC_RegWrite(SlaveAddressIIC, CTRL_REG3, PP_OD_MASK);
            MMA845x_Active();

            /*
            **  Reset FIFO group ID
            */
            value[5] = 0;
          }
          break;

        /***************************************************************************************/
        default:
          /*
          **  Undefined inputs are ignored.
          */
          INPUT_ERROR = TRUE;
          break;
      } //end of case statement for main list of options  
      
      if (INPUT_ERROR == TRUE)
      {
        SCISendString((byte*)string_What);
      }
      SCI_INPUT_READY = FALSE;
      temp = 0;
    }  
    /*
    **  Data streaming is stopped by any character input.
    */
    else
    {
      /*
      **  Turn off data streaming
      */
      
      POLL_ACTIVE = FALSE;
      XYZ_STREAM = FALSE;
      INT_STREAM = FALSE;
      SCI_INPUT_READY = FALSE;
      PROMPT_WAIT = FALSE;

      /*
      **  If samples were being buffered, output them now.
      */
      if (ODR_400 == TRUE) 
      {
        SCI_putCRLF();
        address_out[0] = 0;
        address_out[1] = 0;
        address_out[2] = 0;
        /*
        **  Process FIFO formatted data
        */
        if (functional_block == FBID_FIFO)
        {
          while (ODR_400 == TRUE)
          {
            /*
            **  Read Data Flash samples into the FIFO buffer
            */
            value[0] = RegisterFlag.Byte & F_CNT_MASK;
            for (value[1]=0; value[1]!=value[0]; value[1]++)
            {
              SPI_SS_SELECT;
              SPI_ChrShift(0x0B);
              SPI_ChrShift(address_out[0]);
              SPI_ChrShift(address_out[1]);
              SPI_ChrShift(address_out[2]);
              SPI_ChrShift(0x00);
              fifo_data[value[1]].Sample.XYZ.x_msb = SPI_ChrShiftR(0x00);
              fifo_data[value[1]].Sample.XYZ.x_lsb = SPI_ChrShiftR(0x00);
              fifo_data[value[1]].Sample.XYZ.y_msb = SPI_ChrShiftR(0x00);
              fifo_data[value[1]].Sample.XYZ.y_lsb = SPI_ChrShiftR(0x00);
              fifo_data[value[1]].Sample.XYZ.z_msb = SPI_ChrShiftR(0x00);
              fifo_data[value[1]].Sample.XYZ.z_lsb = SPI_ChrShiftR(0x00);
              SPI_SS_DESELECT;
              /*
              **  Adjust Data Flash address pointer
              */
              if (address_out[2] > 245)
              {
                address_out[2] = 0;
                address_out[1]++;
              }
              else
              {
                address_out[2] += 6;
              }
            }
            /*
            **  Print out the FIFO buffer
            */
            PrintFIFO();
            /*
            **  Check if we're done.
            */
            if (address_out[0] == address_in[0])
            {
              if (address_out[1] == address_in[1])
              {
                if (address_out[2] == address_in[2])
                {
                  ODR_400 = FALSE;
                }
              }
            }
          }
        }
        else
        {
          /*
          **  Read samples stored in Data Flash
          */
          while (ODR_400 == TRUE)
          {
            ReadDataFlashXYZ();
            /*
            **  Output formats are:
            **    - Stream Full Resolution XYZ data as signed counts
            **    - Stream Full Resolution XYZ data as signed g's
            */
            if (STREAM_FULLC == 1) 
            {
            
               //Check Device ID
               switch(deviceID) 
               {
                case 1:
                PrintXYZdec14();            
                break;
                case 2:
                PrintXYZdec12();
                break;
                case 3:
                PrintXYZdec10();
                }
            }
            else
            {
              PrintXYZfrac();
            }
            SCI_putCRLF();
            /*
            **  Adjust Data Flash address pointer
            */
            if (address_out[2] > 245)
            {
              address_out[2] = 0;
              address_out[1]++;
            }
            else
            {
              address_out[2] += 6;
            }
            /*
            **  Check if we're done.
            */
            if (address_out[0] == address_in[0])
            {
              if (address_out[1] == address_in[1])
              {
                if (address_out[2] == address_in[2])
                {
                  ODR_400 = FALSE;
                }
              }
            }
          }
        }
        /*
        **  Erase the Data Flash
        */
        ODR_400 = TRUE;
        SCISendString  ("\r\nErasing Data Flash ... Please wait ");
        ClearDataFlash();
      }
      functional_block = FBID_MAX;
    }
  }
}


/*********************************************************\
**  Terminal Output
\*********************************************************/
void OutputTerminal (byte BlockID, byte *ptr)
{
  switch (BlockID)
  {
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_FULL_XYZ_SAMPLE:
      /*
      **  Full Resolution XYZ Sample Registers (0x00 - 0x06)
      */      
      
      CopyXYZ(ptr); 
      
      /*
      **  If the ODR = 400Hz, buffer the samples in the Data Flash.
      */
      if (ODR_400 == TRUE) 
      {
        /*
        **  Provide a visual indication that we're buffering and write to Flash
        */
        PrintBuffering();
        WriteFlashBuffer();
      }
      else
      {
        /*
        **  Output formats are:
        **    - Stream XYZ data as signed counts
        **    - Stream XYZ data as signed g's
        */
        if (STREAM_FULLC == 1)
        {
        
          switch (deviceID)
          {
            case 1:
            PrintXYZdec14();
            break;
            case 2:
            PrintXYZdec12();
            break;
            case 3:
            PrintXYZdec10();
            break;
          }
        
        }
        else
        {
            PrintXYZfrac();
        }
        
        SCI_putCRLF();
      }
      
      break;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_FIFO:
      /*
      **  FIFO
      **
      **  If the ODR = 400Hz, buffer the samples in the Data Flash.
      */
      if (ODR_400 == TRUE)
      {
        /*
        **  Provide a visual indication that we're buffering
        */
        PrintBuffering();
        /*
        **  Save the samples in the FIFO buffer into the Data Flash
        */
        value[0] = RegisterFlag.Byte & F_CNT_MASK;
        for (value[1]=0; value[1]!=value[0]; value[1]++)
        {
          while (DATAFLASH_Busy() == TRUE);
          /*
          **  Save sample to Data Flash
          */
          DATAFLASH_WriteEnableLatch();
          SPI_SS_SELECT;
          SPI_ChrShift(0x02);
          SPI_ChrShift(address_in[0]);
          SPI_ChrShift(address_in[1]);
          SPI_ChrShift(address_in[2]);
          SPI_ChrShift(fifo_data[value[1]].Sample.XYZ.x_msb);
          SPI_ChrShift(fifo_data[value[1]].Sample.XYZ.x_lsb);
          SPI_ChrShift(fifo_data[value[1]].Sample.XYZ.y_msb);
          SPI_ChrShift(fifo_data[value[1]].Sample.XYZ.y_lsb);
          SPI_ChrShift(fifo_data[value[1]].Sample.XYZ.z_msb);
          SPI_ChrShift(fifo_data[value[1]].Sample.XYZ.z_lsb);
          SPI_SS_DESELECT;
          /*
          **  Adjust Data Flash address pointer
          */
          if (address_in[2] > 245)
          {
            address_in[2] = 0;
            address_in[1]++;
          }
          else
          {
            address_in[2] += 6;
          }
        }
      }
      else
      {
        PrintFIFO();
      }
      break;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    default:
      break;
  }
}


/***********************************************************************************************\
* Private functions
\***********************************************************************************************/

/*********************************************************\
* Print accelerometer's ODR, HP and Mode
\*********************************************************/
void Print_ODR_HP (void)
{
  SCISendString("ODR = ");
  
  if ((IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & MODS_MASK)==0) {
    
    OSMode_Normal=TRUE;
    OSMode_LNLP=FALSE;
    OSMode_HiRes=FALSE;
    OSMode_LP=FALSE;
  } else if ((IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & MODS_MASK)==1) {
    
    OSMode_Normal=FALSE;
    OSMode_LNLP=TRUE;
    OSMode_HiRes=FALSE;
    OSMode_LP=FALSE;
  } else if ((IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & MODS_MASK)==2) {
    
    OSMode_Normal=FALSE;
    OSMode_LNLP=FALSE;
    OSMode_HiRes=TRUE;
    OSMode_LP=FALSE;  
    } else if ((IIC_RegRead(SlaveAddressIIC, CTRL_REG2) & MODS_MASK)==3) {
    OSMode_Normal=FALSE;
    OSMode_LNLP=FALSE;
    OSMode_HiRes=FALSE;
    OSMode_LP=TRUE;
  }
  value[2] = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
  value[0] = value[2] & DR_MASK;
  value[0] >>= 1; // move DR value over to the left by 1

  value[1] = value[0] + (IIC_RegRead(SlaveAddressIIC, HP_FILTER_CUTOFF_REG) & SEL_MASK);
  
  value[0] >>= 2;  // move DR value over to the left by 2 more

  SCISendString((byte*)ODRvalues[value[0]]);
  SCISendString("Hz   ");
  if (value[0] == 0 || value[0]==1)   //Checking for 400Hz ODR or 800Hz ODR
  {
    ODR_400 = TRUE;
  }
  else
  {
    ODR_400 = FALSE;
  }


  SCISendString("HP = ");
  
  if (OSMode_Normal==TRUE) {
    
  SCISendString((byte*)HPvaluesN[value[1]]);
  }
  else if (OSMode_LNLP==TRUE)
  {
   SCISendString((byte*)HPvaluesLNLP[value[1]]);
  }
  else if (OSMode_HiRes==TRUE)
  {
  value[1]=value[1]&0x03;
  SCISendString((byte*)HPvaluesHiRes[value[1]]);
  } 
  else if (OSMode_LP==TRUE){
  SCISendString((byte*)HPvaluesLP[value[1]]);
  }
 
    
  SCISendString("Hz   "); 
  SCISendString("OS Mode = ");
  if (OSMode_Normal==TRUE) 
  {
  SCISendString("Normal   ");
  } 
  else if (OSMode_LNLP==TRUE)
  {
    SCISendString("Low Noise Low Power   ");
  }
  else if (OSMode_HiRes==TRUE){
  SCISendString("Hi Resolution   ");
  } 
  else if (OSMode_LP==TRUE){
  SCISendString("Low Power   ");
  }
   
  SCISendString("G-Range = ");
  SCISendString((byte*)GRange[full_scale]);
}
  

/*********************************************************\
*
\*********************************************************/
void WriteFlashBuffer (void)
{
  /*
  **  Save sample to Data Flash
  */
  DATAFLASH_WriteEnableLatch();
  SPI_SS_SELECT;
  SPI_ChrShift(0x02);
  SPI_ChrShift(address_in[0]);
  SPI_ChrShift(address_in[1]);
  SPI_ChrShift(address_in[2]);
  SPI_ChrShift(x_value.Byte.hi);  
  SPI_ChrShift(x_value.Byte.lo);
  SPI_ChrShift(y_value.Byte.hi);
  SPI_ChrShift(y_value.Byte.lo);
  SPI_ChrShift(z_value.Byte.hi);
  SPI_ChrShift(z_value.Byte.lo);
  SPI_SS_DESELECT;
  /*
  **  Adjust Data Flash address pointer
  */
  if (address_in[2] > 245)
  {
    address_in[2] = 0;
    address_in[1]++;
  }
  else
  {
    address_in[2] += 6;
  }
}


/*********************************************************\
*
\*********************************************************/
void PrintBuffering (void)
{
  if (temp > 50)
  {
    SCI_putCRLF();
    temp = 0;
    address_out[2] += 50;
    if (address_out[2] < 50)
    {
      address_out[1]++;
      if (address_out[1] == 0)
      {
        address_out[0]++;
      }
    }
  }
  if (temp++ == 0)
  {
    hex2ASCII(address_out[0],&value[0]);
    hex2ASCII(address_out[1],&value[2]);
    value[4] = 0;
    SCISendString(&value[0]);
    hex2ASCII(address_out[2],&value[0]);
    value[2] = 0;
    SCISendString(&value[0]);
  }
  SCI_CharOut('*');
}


/*********************************************************\
*
\*********************************************************/
void PrintFIFO (void)
{
  /*
  **  Identify if this is either a FIFO Overflow or Watermark event
  */
  if (RegisterFlag.F_OVF_BIT == 1)
  {
    SCISendString  ("FIFO Overflow ");
  }
  if (RegisterFlag.F_WMRK_FLAG_BIT == 1)
  {
    SCISendString  ("FIFO Watermark ");
  }
  /*
  **  Display FIFO count
  */
  value[0] = RegisterFlag.Byte & F_CNT_MASK;
  hex2ASCII(value[0],&value[1]);
  value[3] = 0;
  SCISendString("0x");
  SCISendString(&value[1]);
  /*
  **  Identify FIFO group
  */
  SCISendString (" group= ");
  hex2ASCII(value[5],&value[1]);
  value[5]++;
  SCISendString(&value[1]);
  SCI_putCRLF();
  /*
  **  Output results
  */
  for (value[1]=0; value[1]!=value[0]; value[1]++)
  {
    CopyXYZ(&fifo_data[value[1]].Sample.XYZ.x_msb);
    /*
    **  Output formats are:
    **    - Stream 14-bit XYZ data as signed counts
    **    - Stream 14-bit XYZ data as signed g's
    */
    if (STREAM_FULLC == 1)
    {
      PrintXYZdec14();
    }
    else
    {
      PrintXYZfrac();
    }
    SCI_putCRLF();
  }
}


/*********************************************************\
*
\*********************************************************/
void ReadDataFlashXYZ (void)
{
  SPI_SS_SELECT;
  SPI_ChrShift(0x0B);
  SPI_ChrShift(address_out[0]);
  SPI_ChrShift(address_out[1]);
  SPI_ChrShift(address_out[2]);
  SPI_ChrShift(0x00);
  x_value.Byte.hi = SPI_ChrShiftR(0x00);
  x_value.Byte.lo = SPI_ChrShiftR(0x00);
  y_value.Byte.hi = SPI_ChrShiftR(0x00);
  y_value.Byte.lo = SPI_ChrShiftR(0x00);
  z_value.Byte.hi = SPI_ChrShiftR(0x00);
  z_value.Byte.lo = SPI_ChrShiftR(0x00);
  SPI_SS_DESELECT;
}


/*********************************************************\
*
\*********************************************************/
byte ProcessHexInput (byte *in, byte *out)
{
  byte data;

  data = *in++;
  if (ishex(data) == TRUE)
  {
    data = tohex(data);
  }
  else
  {
    return (0);
  }
  if (ishex(*in) == TRUE)
  {
    data <<= 4;
    data += tohex(*in);
    *out = data;
    return (2);
  }
  else if ((*in == ' ') || (*in == 0))
  {
    *out = data;
    return (1);
  }
  return (0);
}


/*********************************************************\
*
\*********************************************************/
void CopyXYZ (byte *ptr)
{
  x_value.Byte.hi = *ptr++;
  x_value.Byte.lo = *ptr++;
  y_value.Byte.hi = *ptr++;
  y_value.Byte.lo = *ptr++;
  z_value.Byte.hi = *ptr++; 
  z_value.Byte.lo = *ptr;
       
}

void CopyXYZ8 (byte *ptr) 
{
  x_value.Byte.hi = *ptr++;
  x_value.Byte.lo = 0;
  y_value.Byte.hi = *ptr++;
  y_value.Byte.lo = 0;
  z_value.Byte.hi = *ptr; 
  z_value.Byte.lo = 0;
}
  


/*********************************************************\
*
\*********************************************************/
void PrintXYZdec14 (void)
{
  SCISendString("X= ");
  SCI_s14dec_Out(x_value);
  SCISendString("  Y= ");
  SCI_s14dec_Out(y_value);
  SCISendString("  Z= ");
  SCI_s14dec_Out(z_value);
}
/*********************************************************\
*
\*********************************************************/
 void PrintXYZdec12 (void)
{
  SCISendString("X= ");
  SCI_s12dec_Out(x_value);
  SCISendString("  Y= ");
  SCI_s12dec_Out(y_value);
  SCISendString("  Z= ");
  SCI_s12dec_Out(z_value);
}


/*********************************************************\
*
\*********************************************************/
 void PrintXYZdec10 (void)
{
  SCISendString("X= ");
  SCI_s10dec_Out(x_value);
  SCISendString("  Y= ");
  SCI_s10dec_Out(y_value);
  SCISendString("  Z= ");
  SCI_s10dec_Out(z_value);
}

/*********************************************************\
*
\*********************************************************/
void PrintXYZdec8 (void)
{
  SCISendString("X= ");
  SCI_s8dec_Out(x_value);
  SCISendString("  Y= ");
  SCI_s8dec_Out(y_value);
  SCISendString("  Z= ");
  SCI_s8dec_Out(z_value);
}


/*********************************************************\
*
\*********************************************************/
void PrintXYZfrac (void)
{
  SCISendString("X= ");
  SCI_s14frac_Out(x_value);
  SCISendString("  Y= ");
  SCI_s14frac_Out(y_value);
  SCISendString("  Z= ");
  SCI_s14frac_Out(z_value);
}

/*********************************************************\
*
\*********************************************************/
void ClearDataFlash (void)
{
  address_in[0] = 0;
  address_in[1] = 0;
  address_in[2] = 0;
  address_out[0] = 0;
  address_out[1] = 0;
  address_out[2] = 0;
  DATAFLASH_Erase();
}

