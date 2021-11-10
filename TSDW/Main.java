public class Main {
    public static int x;
    static Mutex mutex;
    public static void main(String[] args) throws InterruptedException {
       x=(int)(Math.random()*11);
       mutex=new Mutex();
       Thread T1,T2;
       T1=new Thread(new Runnable() {
           @Override
           public void run() {
               try {
                T1Method();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
           }
       });
       T2=new Thread(new Runnable() {
           @Override
           public void run() {
               try {
                T2Method();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
           }
       });
       T1.start();
       T2.start();
       T1.join();
       T2.join();
       System.out.println("Avaia");
    }
    private static void T1Method() throws InterruptedException{
        int m;
        while (true){
            m=(int)(Math.random()*11);
            Thread.sleep(100);
            synchronized(mutex){
                if(m==mutex.getX()){
                    System.out.println("Risposta esatta, cristazzo!");
                    mutex.setX();
                    return;
                }
                if(Math.abs(m-mutex.getX())>5){
                    System.out.println("Risposta molto sbagliata, dio merda");
                    mutex.wait();
                }
                else System.out.println("Risposta sbagliata, dio buttana");
            }
        }
    }
    private static void T2Method() throws InterruptedException{
        while (true){
            Thread.sleep(300);
            synchronized(mutex){
                mutex.notifyAll();
                if (mutex.getX()==-1) return;
            }
        }
    }
}
