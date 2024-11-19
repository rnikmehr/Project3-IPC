# compiler
cc := g++

flags := -std=c++17 
LDFLAGS = -lpthread -lrt

# make
csv-server: src/common.cc src/csv-server.cc src/common.cc
	g++ $(flags) $^ -o $@ $(LDFLAGS)

csv-client: src/common.cc src/csv-client-main.cc src/csv-client.cc
	g++ $(flags) $^ -o $@ $(LDFLAGS)

clean:
	rm -r csv-server csv-client *.o
