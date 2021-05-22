

BUILD_FOLDER=build
CLIENT_FOLDER=./src/view
PATH_TO_FX=lib/



all : clean build client


.PHONY : client
client :
	javac -d ${BUILD_FOLDER} --module-path ./lib/ --add-modules javafx.controls,javafx.fxml,javafx.media,javafx.swing   -Xlint ${CLIENT_FOLDER}/*.java
	

run :
	
	cd ${BUILD_FOLDER} && java --module-path ../${PATH_TO_FX} --add-modules javafx.controls,javafx.fxml,javafx.media,javafx.swing view.View
build : 
		mkdir ${BUILD_FOLDER}
		cp -r resources ${BUILD_FOLDER}/resources

clean :
		rm -rf ${BUILD_FOLDER} ./out/logs.txt
		touch ./out/logs.txt

