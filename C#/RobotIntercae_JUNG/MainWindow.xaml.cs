using ExtendedSerialPort;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace RobotIntercae_JUNG
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        bool toogle = true;
        //String receivedText;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM7", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += serialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if (robot.receivedText!= "")
            //{
            //    textBoxReception.Text += robot.receivedText;
            //    robot.receivedText = "";
            //}
            ////foreach (byte value in robot.byteListReceived)
            ////{
            ////    value.ToString();
            ////}
            while (robot.byteListReceived.Count > 0)
            {
                byte value = robot.byteListReceived.Dequeue();
                textBoxReception.Text += "0x" + value.ToString("X2") + " ";
            }
        }

        private void serialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            foreach (byte value in e.Data)
            {
                robot.byteListReceived.Enqueue(value);
            }
        }

        
        //int comptSendmessage;
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (toogle == true)
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.Gray;
            }
            toogle = !toogle;

            SendMessage();

            //comptSendmessage += 1;
            //SendSelectedText = textBoxEmission.Text;
            //textBoxEmission.Text = "";
            //textBoxReception.Text = textBoxReception.Text + "\n" + "Recu : Message "+ comptSendmessage;
        }

        private void SendMessage()
        {
            //textBoxReception.Text += "Recu : "+ receivedText;
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";
           
        }
        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Text = "";
        }
        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //byte[] byteList= new byte[20];
            //for (int i=0; i<20; i++)
            //{
            //    byteList[i] = (byte)(2*i);
            //}
            //serialPort1.Write(byteList, 0, byteList.Length);
            byte[] playload = Encoding.ASCII.GetBytes("Bonjour");
            int playloadLength = playload.Length;
            int fonctionNumber = 0x0080;
            UartEncodeAndSendMessage(fonctionNumber, playloadLength, playload);
        }

        byte CalculateCheckSum(int msgFunction, int msgPlayloadLength, byte[] msgPlayload)
        {
            byte ChekSum = 0x00;
            ChekSum ^= 0xFE;
            ChekSum ^= (byte)(msgFunction >> 8);
            ChekSum ^= (byte)(msgFunction >> 0);
            ChekSum ^= (byte)(msgPlayloadLength >> 8);
            ChekSum ^= (byte)(msgPlayloadLength >> 0);

            for (int i=0; i < msgPlayloadLength; i++)
            {
                ChekSum ^= (byte)(msgPlayload[i]);
            }
            return ChekSum;
        }

        void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, byte[] msgPayload)
        {
            const byte START_BYTE = 0xFE;
            const byte END_BYTE = 0xFF;

            // Calcul de la checksum
            byte checksum = CalculateCheckSum(msgFunction, msgPayloadLength, msgPayload);

            byte[] frame = new byte[7 + msgPayloadLength];
            frame[0] = START_BYTE;
            frame[1] = (byte)(msgFunction >> 8);
            frame[2] = (byte)(msgFunction >> 0);
            frame[3] = (byte)(msgPayloadLength >> 8);
            frame[4]= (byte)(msgPayloadLength >> 0);
            for (int i=0; i < msgPayloadLength; i++)
            {
                frame[5 + i] = (byte)(msgPayload[i]);
            }
            frame[msgPayloadLength + 5] = checksum;
            frame[msgPayloadLength + 6] = END_BYTE;
            serialPort1.Write(frame, 0, frame.Length);
        }

        private void ProcessDecodeMessage(int function, int payloadLength, byte[] payload)
        {
            switch (function)
            {
                case 0x0080:
                    string message = Encoding.ASCII.GetString(payload);
                    serialPort1.Write("message reçu " + message);
                    break;
                default:
                    serialPort1.Write("function non reconnue" + function);
                    break;
            }
            
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            PayLoad,
            CheckSum,
            End
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodeFunction = 0;
        int msgDecodePayLoadLength = 0;
        byte[] msgDecodePayLoad;
        int msgDecodePayloadIndex =0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }break;

                case StateReception.FunctionMSB:
                    msgDecodeFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodeFunction |= c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodePayLoadLength |= c;
                    if (msgDecodePayLoadLength > 0)
                    {
                        msgDecodePayLoad = new byte[msgDecodePayLoadLength];
                        rcvState = StateReception.PayLoad;
                    }
                    else
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;

                case StateReception.PayLoad:
                    msgDecodePayLoad[msgDecodePayloadIndex] = c;
                    msgDecodePayloadIndex++;
                    if (msgDecodePayloadIndex >= msgDecodePayLoadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;

                case StateReception.CheckSum:
                    int calculatedCheckSum = CalculateCheckSum(msgDecodeFunction, msgDecodePayLoadLength, msgDecodePayLoad);
                    if (calculatedCheckSum == c)
                    {
                        //Transmission OK : message valide
                        ProcessDecodeMessage(msgDecodeFunction, msgDecodePayLoadLength, msgDecodePayLoad);
                    }
                    //reset pour la prochaine trame
                    msgDecodeFunction = 0;
                    msgDecodePayLoad = null;
                    msgDecodePayLoadLength = 0;
                    msgDecodePayloadIndex = 0;
                    rcvState = StateReception.End;
                    break;

                case StateReception.End:
                    if (c != 0xFF)
                    {
                        serialPort1.Write(" Message corrompu ");
                        rcvState = StateReception.Waiting;
                    }
                    else
                    {
                        rcvState = StateReception.Waiting;
                    }
                    break;

                default:
                    rcvState = StateReception.Waiting;
                    break;


            }
        }

    }
}
