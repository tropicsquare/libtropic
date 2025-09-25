!!! bug
    The unit tests are outdated and will be redone in the future.

# Unit Tests
Check whether function itself are behaving as should.

## Dependencies
TODO

## Test Organization
TODO

## Run Tests
Unit tests files are in `tests/unit/` folder. They are written in [Ceedling](https://www.throwtheswitch.com) framework, install it like this:

```
    # Install Ruby
    $ sudo apt-get install ruby-full

    # Ceedling install
    $ gem install ceedling

    # Code coverage tool used by Ceedling
    $ pip install gcovr
```

Then make sure that you have correct version:
```
$ ceedling version
   Ceedling:: 0.31.1
      Unity:: 2.5.4
      CMock:: 2.5.4
 CException:: 1.3.3

```
For now we support Ceedling version 0.31.1 only.

Once ceedling is installed, run tests and create code coverage report:

```
$ CEEDLING_MAIN_PROJECT_FILE=scripts/ceedling.yml ceedling gcov:all utils:gcov
```
