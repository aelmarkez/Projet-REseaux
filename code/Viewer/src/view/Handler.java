package view;

import java.io.BufferedReader;
import java.io.IOException;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.LinkedList;

//Classe definit le thread recepteur qui lit la socket et agit à la réponse au serveur.
public class Handler implements Runnable {

    Window window;
    Client client;

    String message;
    BufferedReader in;

    boolean running;
    protected LinkedList<String> History;

    // le constructeur
    public Handler(Window w, BufferedReader in) {
        this.in = in;
        running = true;
        window = w;
        client = window.parser.client;
        History = new LinkedList<String>();
    }

    // Arrête l'execution du thread.
    public void stop() {
        running = false;
    }

    // Agir aux réponses OK
    synchronized private void handleOK() {
        while (History.isEmpty()) {
        }
        String order = History.pop();
        String[] argsOrder = order.split(" |\\, ");
        switch (argsOrder[0]) {
            case "addFish":
                window.view.addFish(argsOrder[1], Integer.parseInt(argsOrder[3].split("x")[0]),
                        Integer.parseInt(argsOrder[3].split("x")[1]), Integer.parseInt(argsOrder[4].split("x")[0]),
                        Integer.parseInt(argsOrder[4].split("x")[1]), false);
                break;
            case "startFish":
                window.view.setStarted(argsOrder[1]);
                break;
            case "delFish":
                window.view.removeFish(argsOrder[1]);
                break;
            default:
                break;
        }
    }

    // La fonction qui va lire le socket et agit qux différents réponse.
    public void run() {
        while (running) {
            try {
                try {
                    message = in.readLine();

                    if (message != null) {
                        window.writeLogs("Receiving : " + message);

                        String[] args = message.split(" |\\[|\\]|\\,");
                        for (String s : args)
                            switch (args[0]) {
                                case "no":
                                    running = false;
                                    break;
                                case "greeting":
                                    window.setId(args[1]);
                                    break;
                                case "NOK":
                                    if (!History.isEmpty()) {
                                        History.pop();
                                    }
                                    break;
                                case "bye":
                                    running = false;
                                    break;
                                case "list":
                                    handleGoal(args);
                                    break;
                                case "OK":
                                    handleOK();
                                    window.display.appendText("< " + s + System.lineSeparator());
                                    break;
                                default:
                                    break;
                            }

                        if (args[0] != "pong") {
                            window.display.appendText("< " + message + System.lineSeparator());
                        }
                    }
                } catch (SocketException e) {
                    running = false;
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    // addFish larabi at 8x8, 4x3, HorizentalWayPoint

    // Agir à la reponse "list" et modifier l'affichage.
    public void handleGoal(String[] args) {

        ArrayList<String> fishesUpdated = new ArrayList<String>();

        for (int i = 2; i < args.length; i = i + 9) {
            String n = args[i];
            fishesUpdated.add(n);
            int x = Integer.parseInt(args[i + 2].split("x")[0]);
            int y = Integer.parseInt(args[i + 2].split("x")[1]);
            int time = Integer.parseInt(args[i + 6]) * 1000;
            int w = Integer.parseInt(args[i + 4].split("x")[0]);
            int h = Integer.parseInt(args[i + 4].split("x")[1]);
            if (!window.view.hasFish(n))
                window.view.addFish(n, x, y, w, h, true);
            else {
                window.view.setFishSize(n, w, h);
                window.view.setGoal(n, x, y, time);
            }
        }
        window.view.removeNonUpdated(fishesUpdated);
    }

}
