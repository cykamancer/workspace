
public class ���ܧ� {
 static String mancer="���ܧ� mancer";
	
	
	

	public ���ܧ�(){
		
		mancer="asd";

		
	}
	
	public static void main (String[] args){
		for(int i=0; i<400; i++){
		char c='��';
		//Character.toChars(c+i);
		//Character.to
		//���ܧ� ���ܧ�=new ���ܧ�();
		//���ܧ�.mancer.
		//���£ãģţƣǣȣɣʣˣ̣ͣΣϣУѣңӣԣգ֣ףأ٣�
		System.out.println(String.copyValueOf(Character.toChars('��'-100+i))+" "+(Character.valueOf('��')-100+i)+" ");
		}
		
	}
};
