all : router

CPPFLAGS= 
SRCS= router.cpp OutgoingSocketManager.cpp
OBJS=$(subst .cpp,.o, $(SRCS))

router: $(OBJS)
	g++ -o router $(OBJS)
depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend