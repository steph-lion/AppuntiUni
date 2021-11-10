public class Cristazzo {
    static int x=0;
    public static void main(String[] args) throws InterruptedException {
        Thread T1=new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i=0;i<10;i++){
                    try {
                        Thread.sleep((long)Math.random()*100);
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                    System.out.println("Sono il thread: T1 e sto incrementando X");
                    increaseX();
                    System.out.println("Valore di X: "+checkX());
                }
            }
        }, "T1");
        Thread T2=new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i=0;i<10;i++){
                    try {
                        Thread.sleep((long)Math.random()*100);
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                    System.out.println("Sono il thread: T2 e sto incrementando X");
                    increaseX();
                    System.out.println("Valore di X: "+checkX());
                }
            }
        }, "T2");
        T1.run();
        T2.run();
        T1.join();
        T2.join();
        System.out.println("Sono il main, ho finito");
    }
    private synchronized static int checkX(){
        return x;
    }
    private synchronized static void increaseX(){
        x++;
    }
}
