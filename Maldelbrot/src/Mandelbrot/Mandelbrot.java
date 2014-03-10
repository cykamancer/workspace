package Mandelbrot;

import java.awt.Color;
import java.awt.Graphics;

import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JPanel;




public class Mandelbrot {
	

    public static int mand(Complex z0) {
        Complex z = z0;
        for (int t = 0; t < 30; t++) {
            if (z.abs() > 2.0) return t;
            z = z.times(z).plus(z0);
        }
        return 30;
    }


	public static void main (String[]args){
		JFrame frame = new JFrame ("");
		frame.setVisible(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(1200,1200);
		JPanel pane = (JPanel) frame.getContentPane();
		pane.add(new mand());
		frame.setVisible(true);	
			
	}}
	class mand extends JComponent{
		/**
		 * 
		 */


		public mand(){
           repaint();
		     
		     
			}
		
	
	public void paint (Graphics g){
		final int SCALE = 200;
		int mand;
		g.setColor(Color.black);
		for(int w=-600; w<600; w++){
			for(int h=-600; h<600; h++){
				mand=mand(new Complex((double)w/SCALE,(double)h/SCALE));
				//if(mand>30){
					//System.out.println(w+" "+h);
					Color color= new Color(10000*mand);
					g.setColor(color);
					g.fillOval(w+600,h+300,2,2);}


		//}
		}

	}


    public static int mand(Complex z0) {
        Complex z = z0;
        for (int t = 0; t < 100; t++) {
            if (z.abs() > 2.0) return t;
            z = z.times(z).plus(z0);
        }
        return 100;
    }
}


