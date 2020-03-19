PIP=pip3
PY=python3
RM=rm -rf

all: requirements module test clean

requirements:
	$(PIP) install setuptools

module:
	$(PY) setup.py build_ext --inplace
	$(PIP) install .

test:
	for tfile in `ls tests/test_*.py`; do \
		$(PY) "$$tfile"; \
	done

clean:
	$(RM) build/ fastrequest/*.so *.data

.PHONY: clean