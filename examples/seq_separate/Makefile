.PHONY: run clean

%.s: %.cvc
	civcc -o $@ $^

%.out: %.s
	civas -o $@ $^

run: main.out seq.out
	civvm $^

clean:
	rm -f *.cpp *.s *.out
