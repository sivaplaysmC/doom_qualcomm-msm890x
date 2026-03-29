
run: push
	ssh phone bash -c 'cd /data/local/tmp && HOME=/data/local/tmp ./doom'

push: doom
	scp ./doom phone:/data/local/tmp/

doom: doom.o display.o PureDOOM.o input.o

clean:
	rm -rf *.o doom

.PHONY: clean push run
