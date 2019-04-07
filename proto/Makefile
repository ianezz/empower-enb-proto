# Copyright (c) 2019 FBK
# Designed by Roberto Riggio
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.

CC=gcc

all:
	$(CC) -I../include -c -Wall -fpic ./$(VERS)/*.c
	$(CC) -shared -o libemproto.so *.o

debug:
	$(CC) -I../include -c -DEBUG -Wall -fpic ./epdbg.c ./$(VERS)/*.c
	$(CC) -shared -o libemproto.so *.o

clean:
	rm -f ./*.o
	rm -f ./*.a
	rm -f ./*.so

install:
	cp ./libemproto.so $(INSTDIR)
	mkdir -p $(INCLDIR)
	cp ../include/emproto.h  $(INCLDIR)/
	cp -r ../include/emproto $(INCLDIR)/

uninstall:
	rm $(INSTDIR)/libemproto.so
	rm -r $(INCLDIR)/emproto
	rm $(INCLDIR)/emproto.h
