import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.io.File;

public class server {

	
	
	public static void main(String[]args){
		
		BufferedReader reader=new BufferedReader(new InputStreamReader(System.in));
		//str=reader.readLine();
		
		
		String str="";
		int noPackets=0;
		int lastAck=0;

		try {
			//Socket socket=new Socket("127.0.0.1", 9876);
			ServerSocket ss=new ServerSocket(9877);
			ServerSocket filess=new ServerSocket(9878);
			System.out.println("server running");			
			Socket socket=ss.accept();
			Socket filesocket=filess.accept();
			System.out.println("connection accepted");

			BufferedReader read_socket=new BufferedReader(new InputStreamReader(socket.getInputStream()));
			DataOutputStream write_socket=new DataOutputStream(socket.getOutputStream());
			DataInputStream read_filesocket=null;
			write_socket.writeBytes("server: enter the number of packets you want to send \r\n");
			noPackets=Integer.parseInt(read_socket.readLine());

			read_filesocket=new DataInputStream(filesocket.getInputStream());
			System.out.println("Outputting to file 'out'...");
			File file = new File("out.txt");
			java.io.FileOutputStream fout = new FileOutputStream(file);
			while(lastAck!=noPackets){

				
				//String from_client=read_socket.readLine();
				//System.out.println(from_client);

    				int len=1024;
    				
    				
    				byte [] b2=new byte[1024];
    				len=read_filesocket.read(b2);
    				System.out.println("file has " +len+" bytes");		
    				fout.write(b2, 0, len);
    				lastAck++;
                    write_socket.writeBytes(lastAck+"\r\n");
    		     	
    				if(len==-1){
    					filesocket.close();
    				}
				}
			    filesocket.close();
				//else{
					
				//	write_socket.writeBytes("no such file\r\n");
				//}
				//str=reader.readLine();
				//write_socket.writeBytes(str+"\r\n");
				
				
				
				
				
				
			
			

			
			
			
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return;
	}
}
