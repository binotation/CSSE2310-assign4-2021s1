import sys
if __name__ == '__main__':
    count = 0
    with open('build/client.out', 'r') as out:
        with open('test/testfiles/spam_client_expected.txt', 'r') as expected:
            for num, expected_line in enumerate(expected):
                out_line = out.readline()
                try:
                    assert expected_line == out_line
                except AssertionError:
                    print(expected_line)
                    print(out_line)
                    print(num)
                    sys.exit(1)
    print("Output matches")
