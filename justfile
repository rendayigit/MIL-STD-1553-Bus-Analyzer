build:
  ./scripts/build.sh

release:
  ./scripts/build.sh --release

clean:
  ./scripts/clean.sh

bc:
  ./scripts/bc.sh

bm:
  ./scripts/bm.sh

rt:
  ./scripts/rt.sh

install:
  ./scripts/install.sh

uninstall:
  ./scripts/uninstall.sh

all: build
rel: release
