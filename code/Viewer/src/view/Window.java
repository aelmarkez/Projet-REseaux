package view;

import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;

import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;

import java.nio.file.Path;
import javafx.stage.Screen;
import javafx.stage.Stage;

import java.io.*;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static javafx.scene.control.Alert.AlertType.INFORMATION;

// Class qui definit la fenêtre où l'utilisateur va communiquer avec le serveur
public class Window extends Stage {

    // lz structure de la fenêtre
    public View view;
    private ToolBar toolbar;
    private String id;

    // l'affichage
    Pane entry;
    VBox vb;
    protected TextArea display;
    protected TextField input;
    private int width;
    private int height;

    // Autres attributs
    Thread continuously;
    protected Parser parser;
    protected Path logsPath = Paths.get(System.getProperty("user.dir") + "/../out/logs.txt");

    // L'hystorique
    protected List<String> history;
    protected int historyCount = 0;

    // Le constructeur.
    public Window(View a, int w, int h) {
        view = a;
        width = w;
        height = h;
        this.setX(Screen.getPrimary().getVisualBounds().getWidth() - width - 100);
        this.setY(100);
        try {
            Files.write(logsPath, Arrays.asList("------------------ Starting the application  ----------: \n"),
                    Charset.forName("UTF-8"));
        } catch (IOException e) {
            System.out.println("Problem while writing in logs : " + e.toString());
        }

        initDisplay();

        parser = new Parser(this, display);

    }

    // Retourne les différents noms des poissons disponibles.
    private String getFishesAvailable() {
        File directory = new File(System.getProperty("user.dir") + "/../resources/images/");
        String[] listFiles = directory.list();
        String res = new String("");
        for (int i = 0; i < listFiles.length; i++) {
            listFiles[i] = listFiles[i].substring(0, listFiles[i].indexOf("."));
            if (!listFiles[i].equalsIgnoreCase("bg")) {
                res += "- " + listFiles[i] + System.lineSeparator();
            }
        }
        return res;
    }

    // Ecrire L'action dans le fichiers de debug.
    protected void writeLogs(String s) {
        try {
            Files.write(logsPath, Arrays.asList("> " + s), Charset.forName("UTF-8"), StandardOpenOption.APPEND);
        } catch (IOException e) {
            System.out.println("Problem while writing in logs : " + e.toString());
        }
    }

    // Initialiser les différents éléments de l'affichage.
    private void initDisplay() {
        this.setTitle("Window");
        entry = new Pane();

        display = new TextArea();
        display.setStyle("-fx-background: green ;");
        display.setEditable(false);

        history = new ArrayList<>();

        initTab();
        initInput();

        vb = new VBox();
        display.setMinHeight(height - input.getHeight() - toolbar.getHeight() - 50);
        display.setMaxHeight(height - input.getHeight() - toolbar.getHeight() - 50);
        vb.getChildren().addAll(toolbar, display, input);
        entry.getChildren().add(vb);

        this.setScene(new Scene(entry, width, height));
    }

    // Initialiser les tab bar.
    private void initTab() {

        Label tab1 = new Label("Fishes");
        initInfoTab(tab1, "Fishes", "Fishes available : ",
                "Fishes available are : " + System.lineSeparator() + getFishesAvailable());

        Label tab2 = new Label("Mobility Models");
        initInfoTab(tab2, "Mobility Models", "Mobility models available : ",
                "Mobility models available are : " + System.lineSeparator() + " - RandomWayPoint "
                        + System.lineSeparator() + " - HorizontalPathWay " + System.lineSeparator()
                        + " - VerticalPathWay " + System.lineSeparator());

        Label tab3 = new Label("Help");
        initInfoTab(tab3, "Help", "Commands available : ",
                "Commands available are : " + System.lineSeparator() + " - hello [in as ID]" + System.lineSeparator()
                        + " - status " + System.lineSeparator() + " - startFish name " + System.lineSeparator()
                        + " - addFish name x y w h modelMoving " + System.lineSeparator() + " - delFish name "
                        + System.lineSeparator() + " - setGoal name x y delay" + System.lineSeparator()
                        + " - getFishes " + System.lineSeparator() + " - getFishesContinuously "
                        + System.lineSeparator());

        toolbar = new ToolBar(tab1, new Separator(), tab2, new Separator(), tab3, new Separator());
    }

    // Initialiser une tab.
    private void initInfoTab(Label l, String title, String header, String content) {
        l.setOnMouseClicked(new EventHandler<MouseEvent>() {
            public void handle(MouseEvent e) {
                if (e.getButton() == MouseButton.PRIMARY) {
                    Alert alert = new Alert(INFORMATION);
                    alert.setHeaderText(header);
                    alert.setTitle(title);
                    alert.setHeight(400);
                    alert.setContentText(content);

                    alert.showAndWait().ifPresent(response -> {
                        if (response == ButtonType.OK) {
                            alert.close();
                        }
                    });
                }
            }
        });
    }

    // Initialiser les différents éléments de la zone d'entrée de commandes.
    private void initInput() {
        input = new TextField();
        input.setPrefColumnCount(20);
        input.addEventHandler(KeyEvent.KEY_RELEASED, keyEvent -> {
            switch (keyEvent.getCode()) {
                case ENTER:
                    String action = input.getText();
                    history.add(action);
                    historyCount = history.size();
                    display.appendText("> " + action + System.lineSeparator());
                    input.clear();

                    writeLogs("New entry : " + action + "/n");
                    parser.parser(action);
                    break;
                case UP:
                    if (historyCount == 0) {
                        break;
                    }
                    historyCount--;
                    input.setText(history.get(historyCount));
                    input.selectAll();
                    break;
                case DOWN:
                    if (historyCount == history.size() - 1) {
                        historyCount = history.size();
                        input.clear();
                        break;
                    }
                    if (historyCount < history.size()) {
                        historyCount++;
                        input.setText(history.get(historyCount));
                        input.selectAll();
                    }
                    break;
                default:
                    historyCount = history.size();
                    break;
            }
        });
    }

    // Permet de savoir si le thread est fini.
    public boolean threadIsOver() {
        return (parser.client.receiver != null && parser.client.receiver.getState() == Thread.State.TERMINATED);
    }

    // Permet de modifier l'identifiant de l'affichage.
    public void setId(String id) {
        this.id = id;
    }

    // Permet de fermer la connexion.
    public void logOut() {
        parser.client.logOut();
        this.close();
    }

}