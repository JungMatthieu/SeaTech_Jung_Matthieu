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
            byte[] byteList= new byte[20];
            for (int i=0; i<20; i++)
            {
                byteList[i] = (byte)(2*i);
            }
            serialPort1.Write(byteList, 0, byteList.Length);
        }

    }
}
