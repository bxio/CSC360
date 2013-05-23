JAVAC=javac
JAR=jar
TARGET=-target 1.1 -source 1.2
CLASSPATH=".;../lib/ClassLinker.jar;../lib"
BOOTCLASSPATH="../lib/java.jar"


#all:  java.jar javax.jar uvic.jar
all:  uvic.jar

java.jar:
	rm -f java/lang/*.class
	rm -f java/io/*.class
	rm -f java/util/*.class
	$(JAVAC) $(TARGET) java/lang/*.java 
	$(JAVAC) $(TARGET) java/io/*.java 
	$(JAVAC) $(TARGET) java/util/*.java 
	$(JAR) cf java.jar java/lang/*.class java/io/*.class java/util/*.class
	cp java.jar ../lib/.

javax.jar:
	rm -f javax/events/*.class
	rm -f javax/memory/*.class
	$(JAVAC) $(TARGET) javax/events/*.java 
	$(JAVAC) $(TARGET) javax/memory/*.java 
	$(JAR) cf javax.jar javax/events/*.class javax/memory/*.class 
	cp javax.jar ../lib/.

uvic.jar:
	rm -f uvic/posix/*.class
	$(JAVAC) $(TARGET) uvic/posix/*.java 
	$(JAR) cf uvic.jar uvic/posix/*.class
	cp uvic.jar ../lib/.

clean:
	rm -f *.jar
