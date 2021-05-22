package view;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import java.awt.*;
import java.io.File;
import java.net.URL;

// Classe qui représent  un poisson
public class Fish {

    // Image du poisson
    private ImageView fishPicture;

    // Attributs du poisson
    private String name;
    private int x;
    private int y;
    private Point goal;
    private long timeGoal;
    private boolean started;
    private double toDoX, toDoY;
    private int doEachX, doEachY;
    private int cpt;
    private long lastElapsed;

    // Constructeur du poisson
    public Fish(String n, int x, int y, int w, int h) {

        initImage(n, w, h);
        setPosition(x, y);
        name = n;
        started = false;
        goal = new Point(-1, -1);
    }

    // Initialise l'image du poisson
    private void initImage(String s, int w, int h) {
        String pictureFile;
        if (s.indexOf('_') == -1) {
            pictureFile = new String(s + ".png");
        } else {
            pictureFile = new String(s.substring(0, s.indexOf('_')) + ".png");
        }
        Boolean exist = false;
        File directory = new File(System.getProperty("user.dir") + "/resources/images");
        String[] listFiles = directory.list();
        for (String f : listFiles) {
            if (f.equalsIgnoreCase(pictureFile) && !f.equalsIgnoreCase("bg.png")) {
                exist = true;
            }
        }
        if (!exist)
            pictureFile = new String("PoissonRouge.png"); // Image par défaut

        if (pictureFile.charAt(pictureFile.length() - 1) == '2') {
            pictureFile = pictureFile.substring(0, pictureFile.length() - 1);
        }

        final URL url = getClass().getResource("/resources/images/" + pictureFile);
        final Image img = new Image(url.toExternalForm());

        fishPicture = new ImageView(img);
        fishPicture.setFitHeight(h);
        fishPicture.setFitWidth(w);
    }

    // Change le booleen started du poisson
    public void setStarted(boolean s) {
        started = s;
    }

    // Renvoie tout les informations sur le poisson
    public String toString() {
        return (name + " at " + x + "x" + y + ", " + fishPicture.getFitWidth() + "x" + fishPicture.getFitHeight()
                + (started ? " started" : " not started"));
    }

    // Met à jour les dimensions du poisson
    public void setSize(int w, int h) {
        fishPicture.setFitHeight(h);
        fishPicture.setFitWidth(w);
    }

    // Met à jour la position du poisson
    public void setPosition(int nx, int ny) {
        x = nx;
        y = ny;
        fishPicture.setX(x);
        fishPicture.setY(y);
    }

    // Renvoie le nom du poisson
    public String getName() {
        return name;
    }

    // Met à jour le goal du poisson

    public void setGoal(int x, int y, long d, long average) {
        timeGoal = d;
        cpt = 0;
        doEachX = 1;
        doEachY = 1;
        goal.x = x;
        goal.y = y;
        double UpdateInit = d / average;
        double Update = d / average;
        this.lastElapsed = average;
        if (Math.abs(x - this.x) < UpdateInit) {
            if (x - this.x == 0) {
                toDoX = 0;
            } else {
                toDoX = (x - this.x) / Math.abs(x - this.x);
            }
        } else {
            toDoX = (x - this.x) / Update;
            while (Math.abs(toDoX) < 1) {
                Update = UpdateInit / doEachX;
                doEachX++;
                toDoX = (x - this.x) / Update;
            }
        }
        Update = d / average;

        if (Math.abs(y - this.y) < UpdateInit) {
            if (y - this.y == 0) {
                toDoX = 0;
            } else {
                toDoX = (y - this.y) / Math.abs(y - this.y);
            }
        } else {
            toDoY = (y - this.y) / Update;
            while (Math.abs(toDoY) < 1) {
                Update = UpdateInit / doEachY;
                doEachY++;
                toDoY = (y - this.y) / Update;
            }
        }
    }

    // Renvoie l'image du poisson
    public ImageView get_View() {
        return fishPicture;
    }

    // Met à jour la position du poisson pour lui permettre d'atteindre son goal
    public void update(long Gone) {

        if (goal.x != -1 || goal.y != -1) {
            timeGoal -= Gone;
            cpt++;

            if (timeGoal <= 1000 && timeGoal > 0) {
                setGoal(goal.x, goal.y, timeGoal, lastElapsed);
            }

            if (timeGoal <= 0) {
                x = goal.x;
                y = goal.y;
                goal.x = -1;
                goal.y = -1;
            } else {
                if (cpt % doEachX == 0) {
                    if (goal.x != x) {
                        if (goal.x < x && x + toDoX < goal.x)
                            x = goal.x;
                        else if (goal.x > x && x + toDoX > goal.x)
                            x = goal.x;
                        else
                            x += toDoX;
                    }
                }

                if (cpt % doEachY == 0) {

                    if (goal.y != y) {
                        if (goal.y < y && y + toDoY < goal.y)
                            y = goal.y;
                        else if (goal.y > y && y + toDoY > goal.y)
                            y = goal.y;
                        else
                            y += toDoY;
                    }
                }

                setPosition(x, y);

                if (goal.x == x && goal.y == y) {
                    goal.x = -1;
                    goal.y = -1;
                }
            }
        }
    }
}
