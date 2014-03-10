package part2;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class client {

	
	
	public static void main(String[]args){
		//Read something.
		BufferedReader reader=new BufferedReader(new InputStreamReader(System.in));
		BufferedReader yesreader=null;
		byte[] b=null;
		//Equivalent to:
		//InputStreamReader is=new InputStreamReader(System.in);
		//BufferedReader reader=new BufferedReader(is);
		String str="";
		String from_server="";
		DataInputStream read_socket=null;
		DataInputStream read_filesocket=null;
		DataOutputStream write_socket=null;


		int sent=1;
		int serverAck=0;
		
		Socket socket = null;
		Socket filesocket = null;
		try {
			socket = new Socket("127.0.0.1", 9876);
			//filesocket = new Socket("127.0.0.1", 9878);
			//read something from socket
			read_socket=new DataInputStream(socket.getInputStream());
			yesreader=new BufferedReader(new InputStreamReader(socket.getInputStream()));
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

		Scanner scr = new Scanner(System.in);
		int noPackets=scr.nextInt();
		try {
			write_socket.writeBytes(noPackets+"\r\n");
			write_socket.writeBytes("0\r\n");
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}

		try {
			


			//from server

			File file = new File("cyka");
			java.io.FileInputStream fin=null;
			DataOutputStream write_filesocket=null;
			if(file.exists()){
				
				fin = new FileInputStream(file);
				write_filesocket = new DataOutputStream(socket.getOutputStream());
				
			}
			while(serverAck!=noPackets){
				
				int len=Math.min(1024, fin.available());
				b = new byte[len];
				fin.read(b,0, len);
				write_filesocket.write(b, 0,len);
				//from_server=yesreader.readLine();
				//System.out.print(from_server);
				//serverAck=Integer.parseInt(from_server);
				
			}
			filesocket.close();		




		} catch (Exception e) {
			// TODO Auto-generated catch block
			System.out.print("error\n");
			e.printStackTrace();
		}
		
		}

		//System.out.print(str);
	

//TO run the server (in the server directory) >> java MainClass
}
