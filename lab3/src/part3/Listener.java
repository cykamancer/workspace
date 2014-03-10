package part3;

import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;

public class Listener implements Runnable{

	Socket socket=null;
	public Listener(Socket socket) {
		 this.socket=socket;
		// TODO Auto-generated constructor stub
	}
//java -cp C:\Users\trolol\workspace\lab3\ARQServer MainClass
	@Override
	public void run() {
		DataInputStream read_socket=null;
		System.out.println("running = ");
		try {
			read_socket=new DataInputStream(socket.getInputStream());
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		// TODO Auto-generated method stub
		while(true){

				//String from_server="0";
				try {
					//System.out.println(read_socket.available());
					client.lastAck = read_socket.read();
					//client.lastAck=Integer.parseInt(from_server);
					//client.lastAck=from_server;
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				System.out.println("from_server = "+client.lastAck);
				client.Ackupdated=true;
            	System.out.println(client.wSize);
				//client.lastAck=Integer.parseInt(from_server);
			
		}
		
	}

}
