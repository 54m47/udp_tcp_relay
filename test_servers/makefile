CC = gcc
CFLAGS = -Wall -Wextra

SRCDIR = src
INCDIR = include
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

UDPCLIENT = udp_client
TCPSERVER = tcp_server

all: $(UDPCLIENT) $(TCPSERVER)

$(UDPCLIENT): $(OBJDIR)/udp_client.o
	$(CC) $(CFLAGS) $^ -o $@

$(TCPSERVER): $(OBJDIR)/tcp_server.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: run_tcpserver
	./$(UDPCLIENT)

run_tcpserver: $(TCPSERVER)
	./$(TCPSERVER) &

stop:
	pkill -f "./$(TCPSERVER)"

clean:
	rm -rf $(OBJDIR) $(UDPCLIENT) $(TCPSERVER)