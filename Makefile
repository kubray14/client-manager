all:
	chmod a+x manager.c
	gcc -o manager manager.c
	gcc -o client client.c
	
clean:
	rm manager
	rm client
