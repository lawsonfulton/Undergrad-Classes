all: sender.class receiver.class
sender.class: sender.java
	javac -d . -classpath . sender.java packet.java MyUtil.java
receiver.class: receiver.java
	javac -d . -classpath . receiver.java packet.java MyUtil.java
clean:
	rm -f *.class 
