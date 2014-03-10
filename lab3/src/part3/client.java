package part3;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class client {

	
	static int lastAck=0;
	static boolean Ackupdated=true;
	static BufferedReader yesreader=null;
	static int wSize=4;
	public static void main(String[]args){
		//Read something.
		BufferedReader reader=new BufferedReader(new InputStreamReader(System.in));
		//BufferedReader yesreader=null;
		byte[] b=null;
		//Equivalent to:
		//InputStreamReader is=new InputStreamReader(System.in);
		//BufferedReader reader=new BufferedReader(is);
		String str="";
		String from_server="";
		
		int timeOut=10000;
		
		//DataInputStream read_socket=null;
		DataInputStream read_filesocket=null;
		DataOutputStream write_socket=null;


		int sent=1;
		
		Socket socket = null;
		Socket filesocket = null;
		try {
			socket = new Socket("127.0.0.1", 9876);
			//filesocket = new Socket("127.0.0.1", 9878);
			//read something from socket
			//read_socket=new DataInputStream(socket.getInputStream());
			yesreader=new BufferedReader(new InputStreamReader(socket.getInputStream()));
			//yeswriter=new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
			//write something to socket
			write_socket=new DataOutputStream(socket.getOutputStream());

			b=new byte[1024];

		} catch (UnknownHostException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		//System.out.println("Server: "+from_server);
		System.out.println("connected\n");
		Thread thread=new Thread(new Listener(socket));
		thread.start();
		//Scanner scr = new Scanner(System.in);
		//int noPackets=scr.nextInt();
		int noPackets=10;

		try {
			write_socket.write(noPackets);
			write_socket.write(0);
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}

		try {
		
			
			
			double[] timer=null;
			timer=new double [wSize];
			while(lastAck<noPackets){
				//System.out.println("from_server = "+lastAck+" "+noPackets);

                if(Ackupdated){
                	wSize=wSize*2;
                	Ackupdated=false;
                }
                double a=System.currentTimeMillis()-timer[0];
                timer=new double [wSize];
                //System.out.println(a);
                
                if(a>timeOut){
                	//sent=sent-wSize;
                	wSize=4; 
                	
                }
                for(int i=0; i<wSize; i++){
                	write_socket.write(sent);

                	//if(lastAck<noPackets){
                    //write_socket.write(sent);}
                    timer[(noPackets%wSize)-1]=System.currentTimeMillis();
                    sent++;
                    System.out.println("from_server = "+lastAck+" "+noPackets);
                    if(lastAck==noPackets) {break;}
                }
                
                if(lastAck==noPackets) {break;}
                //for(int i=0; i<wSize; i++){
                /*
				int len=Math.min(8, fin.available());
				b = new byte[len];
				fin.read(b,0, len);
				write_socket.write(b, 0,len);
				
				     timer[(noPackets%wSize)-1]=System.currentTimeMillis();
                //}
				     //System.out.println(timer[0]);
				//from_server=yesreader.readLine();
				//System.out.print(from_server);
				//serverAck=Integer.parseInt(from_server);*/
            	//System.out.println("EOF");
			}
			//thread.destroy();
			//filesocket.close();		

			System.out.println("EOC");


		} catch (Exception e) {
			// TODO Auto-generated catch block
			System.out.print("error\n");
			e.printStackTrace();
		}
		
		}

		//System.out.print(str);
	

//TO run the server (in the server directory) >> java MainClass
}
