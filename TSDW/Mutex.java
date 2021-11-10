public class Mutex {
    static int x;
    public Mutex(){
        x=(int)(Math.random()*11);
    }
    public int getX(){
        return x;
    }
    public void setX(){
        x=-1;
    }
}
