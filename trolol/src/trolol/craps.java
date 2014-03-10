package trolol;
import java.util.Random;

public class craps {

	public enum Status {CONTINUE, WON, LOST}
	
	public static void main(String[]args){
		
		int myPoint=0;
		Status gameStatus;
	    int sumofdice=rolldice();
	    switch(sumofdice){
	    case 7:
	    case 11:
	        gameStatus=Status.WON;
	        break;
	    case 2:
	    case 3:
	    case 12:
	    	gameStatus=Status.LOST;
	    	break;
	    default:
	    	gameStatus=Status.CONTINUE;
	    	myPoint=sumofdice;
	    	System.out.println("Point is "+myPoint+".");
	    	break;
	    }
	    while (gameStatus==Status.CONTINUE){
	    	sumofdice=rolldice();
	    	if(sumofdice==myPoint)
	    		gameStatus=Status.WON;
	    	else if(sumofdice==7)
	    		gameStatus=Status.LOST;
	    }
		if(gameStatus==Status.WON)
			System.out.println("Player wins");
		else
			System.out.println("Player loses");
	}
	
	public static int rolldice(){
		int die1=(new Random()).nextInt(6)+1;
		int die2=(new Random()).nextInt(6)+1;
		int sum=die1+die2;
		System.out.println("Player rolled "+die1+" + "+die2+" = "+sum);
		return sum;
		
	}
}
