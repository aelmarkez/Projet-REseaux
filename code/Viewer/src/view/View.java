package view;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import javafx.scene.layout.Pane;
import javafx.stage.Stage;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.*;

// Classe représentant un affichage .
public class View extends Application {

    // Attributs de configuration
    private String id;
    private long pingTimeslice;
    private String imagesURL;
    protected long timeGone = 0;
    protected long timeSinceStart = 0;

    // Attributs d'animation
    private int timer = 0;
    private long previousTime = 0;

    // Attributs d'affichage
    protected int width = 650;
    protected int height = 400;
    private final Pane view = new Pane();
    private ImageView bgPicture;

    // Attributs de manipultion de notre affichage
    private ArrayList<Fish> fishes;
    private Window wind;
    private boolean newFishAdded = false;

    // Lance l'application
    public void start(Stage primaryStage) throws Exception {
        // chemin vers l'image de background
        String backGroundImage = "/resources/images/bg.png/";

        // Liste des poissons de la vue
        fishes = new ArrayList<Fish>();

        // La wind d'échange avec l'utilisateur
        wind = new Window(this, 400, 400);

        wind.show();
        config();
        wind.input.setText("hello in as N1");

        // Initialisation de la vue
        final URL backGroundUrl = getClass().getResource(backGroundImage);
        final Image bg = new Image(backGroundUrl.toExternalForm());
        bgPicture = new ImageView(bg);
        bgPicture.setFitHeight(height);
        bgPicture.setFitWidth(width);
        view.getChildren().setAll(bgPicture);
        view.getChildren().addAll(getAllViews());

        primaryStage.setResizable(false);
        primaryStage.setTitle("View");
        primaryStage.setMaxHeight(height);
        primaryStage.setMaxWidth(width);

        primaryStage.setScene(new Scene(view, width, height));
        primaryStage.setX(300);
        primaryStage.setY(300);
        Platform.setImplicitExit(false);
        primaryStage.setOnCloseRequest(event -> {
            wind.parser.client.handler.stop();
            wind.logOut();
            System.exit(1);
        });
        primaryStage.show();

        handler();
    }

    // Définie les actions à gérer tout les ticks
    private void handler() {
        new AnimationTimer() {
            @Override
            public void handle(long now) {
                timer++;
                ;

                if (wind.threadIsOver() || !wind.parser.client.isConnected()) {
                    wind.writeLogs("Programme finished.");
                    wind.logOut();
                    System.exit(1);
                }

                if (newFishAdded) {
                    newFishAdded = false;
                    view.getChildren().remove(1, view.getChildren().size());
                    view.getChildren().addAll(getAllViews());
                }

                if (previousTime != 0) {
                    timeGone += previousTime;
                    timeSinceStart += (now / 1000000) - previousTime;
                    for (Fish f : fishes) {
                        f.update((now / 1000000) - previousTime);
                    }
                }

                if (timeGone >= pingTimeslice * 10000000) {
                    timeGone = 0;
                    wind.parser.client.send("ping 12345");
                    wind.writeLogs("ping 12345");
                }
                if (timer % 30 == 0) {
                    view.getChildren().remove(1, view.getChildren().size());
                    view.getChildren().addAll(getAllViews());
                }

                previousTime = now / 1000000;
            }
        }.start();
    }

    // Initialise les différents attributs de configuration
    public void config() {
        Properties prop = new Properties();
        InputStream is = null;
        try {
            is = new FileInputStream(System.getProperty("user.dir") + "/resources/affichage.cfg");
            prop.load(is);

            String address = prop.getProperty("controller-address");
            int port = Integer.parseInt(prop.getProperty("controller-port"));
            id = prop.getProperty("id");

            pingTimeslice = Integer.parseInt(prop.getProperty("display-timeout-value"));
            imagesURL = prop.getProperty("resources");

            wind.writeLogs("Configurate client :\n ID=" + id + "\n server adress=" + address + "\n Port=" + port
                    + "\n default Timeout=" + pingTimeslice + " millisecondes\n Image directory=" + imagesURL + "\n");
            wind.parser.client.configurate(address, port);
        } catch (IOException e) {
            System.out.println("Exception while reading affichage.cfg : " + e.toString());
            wind.writeLogs("Error while reading affichage.cfg.\n");
        }

    }

    // Met à jour les dimensions du poisson du nom "name"
    public void setFishSize(String name, int w, int h) {
        for (Fish f : fishes) {
            if (f.getName().contentEquals(name)) {
                f.setSize(w * width / 100, h * height / 100);
            }
        }
    }

    // Vérifie si le poisson du nom "name" se trouve dans la vue
    public boolean hasFish(String name) {
        for (Fish f : fishes) {
            if (f.getName().contentEquals(name))
                return true;
        }
        return false;
    }

    // Renvoie le nombre des poissons dans la vue
    public int getNbFishes() {
        return fishes.size();
    }

    // Renvoie tout les images des poissons de la vue
    public Collection<ImageView> getAllViews() {
        ArrayList<ImageView> res = new ArrayList<ImageView>();
        for (Fish f : fishes) {
            res.add(f.get_View());
        }
        return res;
    }

    // Ajoute le poisson du nom "name" dans la vue.
    public void addFish(String name, int x, int y, int w, int h, boolean started) {
        Fish f = new Fish(name, x * width / 100, y * height / 100, w * width / 100, h * height / 100);
        f.setStarted(started);
        fishes.add(f);
        newFishAdded = true;
    }

    // Active le booleen started du poisson du nom "name".
    public void setStarted(String name) {
        for (Fish f : fishes) {
            if (f.getName().contentEquals(name)) {
                f.setStarted(true);
            }
        }
    }

    // Supprime le poisson du nom "name" de la vue.
    public void removeFish(String name) {
        Fish toRemove = null;
        for (Fish f : fishes) {
            if (f.getName().equalsIgnoreCase(name)) {
                toRemove = f;
            }
        }
        if (toRemove != null)
            fishes.remove(toRemove);
    }

    // Définie un nouveau goal pour le poisson du nom "name"
    public void setGoal(String name, int x, int y, long d) {
        for (Fish f : fishes) {
            if (f.getName().equalsIgnoreCase(name)) {
                wind.writeLogs("New Goal for the fish" + name + " : aller en " + x + "x" + y + " en " + d
                        + " millisecondes/n");
                f.setGoal(x * width / 100, y * height / 100, d, timeSinceStart / timer);
            }
        }
    }

    // Supprime tout les poissons qui ne sont pas mis à jour.
    public void removeNonUpdated(ArrayList<String> fishesUpdated) {
        ArrayList<Fish> toRemove = new ArrayList<Fish>();
        for (Fish f : fishes) {
            if (!fishesUpdated.contains(f.getName()))
                toRemove.add(f);
        }

        for (Fish f : toRemove) {
            fishes.remove(f);
        }

    }

    // Retourne la liste des poissons qui se trouvent dans la vue
    public String toString() {
        String s = new String(fishes.size() + " fishes found" + System.lineSeparator());

        for (Fish f : fishes) {
            s += "Fish " + f.toString() + System.lineSeparator();
        }

        return s;
    }

    // Le main du programme.
    public static void main(String[] args) {
        launch(args);
    }
}
