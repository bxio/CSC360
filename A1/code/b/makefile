JAVAC=javac
JAVA=java
CLASSPATH=".;../lib/ClassLinker.jar;../lib/uvic.jar;../lib"
BOOTCLASSPATH="../lib/java.jar"
TARGET=-target 1.1 -source 1.2

%.bin : %.java
	$(JAVAC) $(TARGET) -classpath $(CLASSPATH) -bootclasspath $(BOOTCLASSPATH) $<
	$(JAVA) -cp $(CLASSPATH) ClassLinker -f $*
	#rm -f $*.hex *.class lookup.c

all : LockRace.bin

clean:
	rm -f *.class *.hex *.bin *.map lookup.c
