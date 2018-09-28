all: client.class server.class
client.class: client.java
	javac -d . -classpath . client.java
server.class: server.java
	javac -d . -classpath . server.java
clean:
	rm -f *.class 
