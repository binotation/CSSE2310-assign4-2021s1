if __name__ == '__main__':
    with open('load_test.sh', 'w') as f:
        for i in range(2500):
            f.write('cat test/testfiles/spam2.txt | build/client Barry test/testfiles/auth.txt 10754 > /dev/null &\n')
