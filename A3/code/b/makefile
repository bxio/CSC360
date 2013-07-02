JAVAC=javac
JAVA=java
CLASSPATH=".;../lib/ClassLinker.jar;../lib/uvic.jar;../lib"
BOOTCLASSPATH="../lib/java.jar"
TARGET=-target 1.1 -source 1.2

%.bin : %.java
	$(JAVAC) $(TARGET) -encoding ISO-8859-1 -classpath $(CLASSPATH) -bootclasspath $(BOOTCLASSPATH) $<
	$(JAVA) -cp $(CLASSPATH) ClassLinker -f $*
	#rm -f $*.hex *.class lookup.c

all : TestRead.bin TestWrite.bin TestDelete.bin 

clean:
	rm -f *.class *.hex *.bin *.list *.map lookup.c
