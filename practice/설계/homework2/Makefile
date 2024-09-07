CC = gcc

HEADER = ssu_header
BACKUP = ssu_repo
HELP = ssu_help

$(BACKUP): $(BACKUP).o $(HELP).o
	$(CC) -o $@ $(BACKUP).o $(HELP).o -lcrypto
$(BACKUP).o: $(HEADER).h $(BACKUP).c
	$(CC) -c -o $@ $(BACKUP).c

$(HELP).o: $(HELP).c
	$(CC) -c -o $@ $(HELP).c



clean :
	rm -rf $(BACKUP)
	rm -rf *.o

.PHONY: clean

