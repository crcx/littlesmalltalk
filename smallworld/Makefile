all: SmallWorld SmallWorld.tar SmallWorld.jar

SmallWorld: SmallWorld.java SmallObject.java
	javac SmallObject.java SmallWorld.java
	chmod 644 *.java *.class

SmallWorld.tar: SmallWorld.java SmallObject.java image
	tar cvf SmallWorld.tar Makefile SmallWorld.mf SmallWorld.java SmallObject.java image *.class
	chmod 644 SmallWorld.tar

SmallWorld.jar: SmallWorld.class SmallObject.class
	jar cmf SmallWorld.mf SmallWorld.jar *.class
	chmod 644 SmallWorld.jar
