#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>

void set_random_seed();
int randn(int n);

struct mm_code_maker{

    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;
    }

    void generate_sequence(){
        for(int i = 0; i < length; i++){
            sequence.push_back(randn(num));
        }
    }

    void give_feedback(const std::vector<int>& attempt, int& black_hits, int& white_hits){
      black_hits = 0;
      white_hits = 0;
      int whitetemp = 0;
      std::vector<int> sequencechecker(num, 0);
      std::vector<int> attemptchecker(num, 0);

      for(int i = 0; i < length; i++){
        if(sequence[i] == attempt[i]){
          black_hits++;
        }
        sequencechecker[sequence[i]]++;
        attemptchecker[attempt[i]]++;
      }
      for(int i = 0; i < num; i++){
        if(sequencechecker[i] <= attemptchecker[i]){
          whitetemp = whitetemp + sequencechecker[i];
        }
        else{
          whitetemp = whitetemp + attemptchecker[i];
        }
      }
      white_hits = whitetemp - black_hits;
    }

    std::vector<int> sequence;
    int length;
    int num;
};

struct mm_solver{

  // list of general variables used by ALL methods
  int length;
  int num;
  int setlength;
  int counter = 0;
  int value = 10000; // predetermined, set by experiment
  int indexbound = 0;
  bool speedup = false;
  bool swaszekmode = true;
  bool gameover = false;
  bool initialattempt = true;
  bool knuthmode = false;
  std::vector<int> freevector;
  std::vector<int> freevectorchecker;
  std::vector<int> nextattempt;
  std::vector<int> attemptchecker;
  std::vector<int> numinitializer;
  std::vector<int> segmentinitializer;
  std::vector<std::vector<int>> codeusedlist;
  std::vector<std::vector<int>> codeusedlistBW;
  std::vector<std::vector<int>> possiblevalues;

  // list of variables used for KNUTH method
  bool initialsetup = true;
  std::vector<int> codecomparechecker;

  // list of variables used for SWASZEK and SPLIT methods
  int pointer;
  int blackhitsegment;
  int newblackstart = 0;
  int segment, segment1, segment2;
  bool usewhite = false;
  bool foundzeroblack = false;
  bool movenextsegment = false;
  bool foundsegment = true;
  std::vector<int> nextattemptstore;
  std::vector<int> attemptsegment;

  void init(int i_length, int i_num){
      length = i_length;
      num = i_num;

      // ENTER KNUTH ALGORITHM
      if(std::pow(num, length) < 5000000){
        knuthmode = true;
        std::vector<int> lengthinitializertemp(length, 0);
        std::vector<int> numinitializertemp(num, 0);
        freevector = lengthinitializertemp;
        numinitializer = numinitializertemp;
      }

      // ENTER SPLIT ALGORITHM
      else if(std::pow(num, length) > std::pow(6, 15) + 1 || (length > 10 && num > 10)){
        swaszekmode = false;
        segment1 = length/2;
        segment2 = length - segment1;
        segment = segment1; // initialize to first segment
        pointer = 0;
        std::vector<int> segmentinitializertemp(segment, 0);
        std::vector<int> numinitializertemp(num, 0);
        freevector = segmentinitializertemp;
        attemptsegment = segmentinitializertemp;
        numinitializer = numinitializertemp;
      }

      // ENTER SWASZEK ALGORITHM
      else{
        swaszekmode = true;
        std::vector<int> lengthinitializertemp(length, 0);
        std::vector<int> numinitializertemp(num, 0);
        freevector = lengthinitializertemp;
        numinitializer = numinitializertemp;
      }
    }

    void create_attempt(std::vector<int>& attempt){
      // SWASZEK METHOD
      if(swaszekmode && !knuthmode){
        if(initialattempt){
          for(int i = 0; i < length; i++){
              attempt.push_back(randn(num));
          }
          initialattempt = false;
        }
        else{
          attempt = nextattempt;
        }
      }

      // SPLIT METHOD
      else if(!swaszekmode && !knuthmode){
        if(!foundzeroblack){
          std::vector<int> randomguess;
          for(int i = 0; i < length; i++){
            randomguess.push_back(randn(num));
          }
          attempt = randomguess;
        }
        else{
          int j = 0;
          for(int i = pointer; i < pointer + segment; i++){
            nextattemptstore[i] = nextattempt[j];
            j++;
          }
          attempt = nextattemptstore;
        }
      }

      // KNUTH METHOD
      else{
        if(initialattempt){
          for(int i = 0; i < length; i++){
              attempt.push_back(randn(num));
          }
          initialattempt = false;
        }
        else{
          int random = randn(possiblevalues.size());
          attempt = possiblevalues[random];
        }
      }
    }

    void learn(std::vector<int>& attempt, int black_hits, int white_hits){
      // EMTER SPLIT and SWASZEK CONDITION
      if(!knuthmode){
        if(black_hits == length){
          gameover = true; // to prevent a crash after final code is found (when learn is called)
        }
        nextattemptstore = attempt;
        counter = 0;
        if(!swaszekmode){
          blackhitsegment = black_hits - newblackstart;
          if(!foundzeroblack){
            if(black_hits == 0){
              foundzeroblack = true;
            }
            foundsegment = true;
          }
          if(blackhitsegment == segment && foundzeroblack){
            foundsegment = true;
          }
          if(foundsegment && foundzeroblack){
            if(movenextsegment){ // initially, remain at segment 1
              pointer = pointer + segment;
              segment = segment2;
              usewhite = true;
              std::vector<int> segmentinitializertemp(segment, 0);
              freevector = segmentinitializertemp;
              segmentinitializer = segmentinitializertemp;
              attemptsegment = segmentinitializertemp;
            }
            else{
              movenextsegment = true;
            }
            codeusedlist.clear();
            codeusedlistBW.clear();
            newblackstart = black_hits;
            blackhitsegment = 0;
            foundsegment = false;
          }
          int j = 0;
          for(int i = pointer; i < segment + pointer; i++){
            attemptsegment[j] = attempt[i];
            j++;
          }
        }

        freevectorchecker = numinitializer;
        attemptchecker = numinitializer;
        if(!swaszekmode && foundzeroblack){
          std::vector<int> BWtemp;
          BWtemp.push_back(blackhitsegment);
          if(usewhite){
            BWtemp.push_back(white_hits);
          }
          codeusedlistBW.push_back(BWtemp);
          codeusedlist.push_back(attemptsegment);
        }
        if(swaszekmode){
          std::vector<int> BWtemp;
          BWtemp.push_back(black_hits);
          BWtemp.push_back(white_hits);
          codeusedlist.push_back(attempt);
          codeusedlistBW.push_back(BWtemp);
        }
        if((swaszekmode || (!swaszekmode && foundzeroblack)) && !gameover){
          if(nextstep()){
            if(speedup){
              std::vector<int> speeduptemp = freevector;
              setlength = length;
              if(!swaszekmode){
                setlength = segment;
              }
              int upperlimit = setlength - indexbound;
              int lowerlimit = indexbound;
              for(int i = 0; i < upperlimit; i++){
                speeduptemp[rand() % upperlimit + lowerlimit] = randn(num);
              }
              nextattempt = speeduptemp;
              speedup = false;
            }
            else{
              nextattempt = freevector;
            }
          }
        }
      }

      // ENTER KNUTH CONDITION
      else{
        if(initialsetup == true){
          freevectorchecker = numinitializer;
          attemptchecker = numinitializer;
          while(nextinitial(black_hits, white_hits, attempt)){
            possiblevalues.push_back(freevector);
          }
        }
        else{
          std::vector<std::vector<int>> newpossiblevalues;
          attemptchecker = numinitializer;
          for(int i = 0; i < possiblevalues.size(); i++){
            if(codecomparator(attempt, black_hits, white_hits, i)){
              newpossiblevalues.push_back(possiblevalues[i]);
              attemptchecker = numinitializer;
            }
            attemptchecker = numinitializer;
          }
          possiblevalues = newpossiblevalues;
        }
      }
    }

    // ONLY FOR SWASZEK AND SPLIT METHOD
    bool nextstep(){
      int index, indexmarker;
      if(!swaszekmode){
        indexmarker = segment + 1;
        index = indexmarker;
        setlength = segment;
      }
      else{
        indexmarker = length + 1;
        index = indexmarker;
        setlength = length;
      }
      while(nextstepcompare(index, indexmarker) == false){
        counter++;
        freevector[index]++;
        if(freevector[index] == num){
          while(freevector[index] == num && index > 0){
            freevector[index] = 0;
            freevector[index - 1]++;
            index--;
          }
          if(index == 0 && freevector[index] == num){
            freevectorchecker = numinitializer;
            attemptchecker = numinitializer;
            return false; // indicates end of possible codes
          }
        }
        for(int i = index + 1; i < setlength; i++){
          freevector[i] = 0;
        }
        index = indexmarker;
        freevectorchecker = numinitializer;
        attemptchecker = numinitializer;
      }
      freevectorchecker = numinitializer;
      attemptchecker = numinitializer;
      return true;
    }

    // ONLY FOR SWASZEK AND SPLIT METHOD
    bool nextstepcompare(int& index, int indexmarker){
      for(int z = 0; z < codeusedlist.size(); z++){
        freevectorchecker = numinitializer;
        attemptchecker = numinitializer;
        bool leaveloop = false;
        int currentblackpegs = 0;
        int white_hits;
        int whitetemp = 0;
        int lengthfromend = length;
        setlength = length;
        if(!swaszekmode){
          lengthfromend = segment;
          setlength = segment;
        }
        std::vector<int> attempt = codeusedlist[z];
        int black_hits = codeusedlistBW[z][0];
        if(swaszekmode || usewhite){
          white_hits = codeusedlistBW[z][1];
        }
        for(int i = 0; i < setlength && leaveloop == false; i++){
          lengthfromend--;
          if(freevector[i] == attempt[i]){
            currentblackpegs++;
          }
          if(currentblackpegs > black_hits || currentblackpegs + lengthfromend < black_hits){
            if(i < index){
              index = i;
            }
            leaveloop = true;
          }
          if(swaszekmode || usewhite){
            freevectorchecker[freevector[i]]++;
            attemptchecker[attempt[i]]++;
            if(attempt[i] == freevector[i]){
              whitetemp++;
            }
            else{
              if(freevectorchecker[freevector[i]] <= attemptchecker[freevector[i]]){
                whitetemp++;
              }
              if(attemptchecker[attempt[i]] <= freevectorchecker[attempt[i]]){
                whitetemp++;
              }
            }
            if(whitetemp - currentblackpegs > white_hits || whitetemp - currentblackpegs + 2*lengthfromend < white_hits){
              if(i < index){
                index = i;
              }
              leaveloop = true;
            }
          }
        }
      }
      if(index < indexmarker){ // enter if free vector is not a possible code
        if(counter > value){
          if(value > 1500){
            value = value - 150;
          }
          indexbound = index;
          speedup = true;
          return true; // return true to indicate possible code is found
        }
        return false;
      }
      if(index == indexmarker){ // enter if free vector is a possible code
        return true;
      }
    }

    // ONLY FOR KNUTH METHOD
    bool nextinitial(int black_hits, int white_hits, const std::vector<int>& attempt){
      int index = 0;
      if(initialsetup == true){
        initialsetup = false; // so doesn't skip 00000000...
      }
      else{
        freevector[length - 1]++; // increments free vector by 1
        if(freevector[length - 1] == num){
          int indexfix = length - 1;
          while(freevector[indexfix] == num && indexfix > 0){
            freevector[indexfix] = 0;
            freevector[indexfix - 1]++;
            indexfix--;
          }
          if(indexfix == 0 && freevector[indexfix] == num){
            return false; // this indicates end of possibilities
          }
        }
      }
      while(nextinitialcompare(attempt, index, black_hits, white_hits) == false){
        freevector[index]++;
        if(freevector[index] == num){
          while(freevector[index] == num && index > 0){
            freevector[index] = 0;
            freevector[index - 1]++;
            index--; // move to previous index
          }
          if(index == 0 && freevector[index] == num){
            freevectorchecker = numinitializer;
            attemptchecker = numinitializer;
            return false; // this indicates end of possibilities
          }
        }
        index = 0;
        freevectorchecker = numinitializer;
        attemptchecker = numinitializer;
      }
      freevectorchecker = numinitializer;
      attemptchecker = numinitializer;
      return true;
    }

    // ONLY FOR KNUTH METHOD
    bool nextinitialcompare(const std::vector<int>& attempt, int& index, int black_hits, int white_hits){
      int currentblackpegs = 0;
      int lengthfromend = length;
      int whitetemp = 0;
      for(int i = 0; i < length; i++){
        lengthfromend--;
        if(freevector[i] == attempt[i]){
          currentblackpegs++;
        }
        if(currentblackpegs > black_hits || currentblackpegs + lengthfromend < black_hits){
          index = i;
          return false;
        }
        freevectorchecker[freevector[i]]++;
        attemptchecker[attempt[i]]++;
        if(attempt[i] == freevector[i]){
          whitetemp++;
        }
        else{
          if(freevectorchecker[freevector[i]] <= attemptchecker[freevector[i]]){
            whitetemp++;
          }
          if(attemptchecker[attempt[i]] <= freevectorchecker[attempt[i]]){
            whitetemp++;
          }
        }
        if(whitetemp - currentblackpegs > white_hits || whitetemp - currentblackpegs + lengthfromend*2 < white_hits){
          index = i;
          return false;
        }
      }
      return true;
    }

    // ONLY FOR KNUTH METHOD
    bool codecomparator(const std::vector<int>& attempt, int black_hits, int white_hits, int counter){
      int currentblackpegs = 0;
      int lengthfromend = length;
      int whitetemp = 0;
      std::vector<int> codecompare = possiblevalues[counter];
      std::vector<int> codecomparechecker(num, 0);
      for(int i = 0; i < length; i++){
        lengthfromend--;
        if(codecompare[i] == attempt[i]){
          currentblackpegs++;
        }
        if(currentblackpegs > black_hits || currentblackpegs + lengthfromend < black_hits){
          return false;
        }
      }
      lengthfromend = length;
      for(int i = 0; i < length; i++){
        lengthfromend--;
        codecomparechecker[codecompare[i]]++;
        attemptchecker[attempt[i]]++;
        if(attempt[i] == codecompare[i]){
          whitetemp++;
        }
        else{
          if(codecomparechecker[codecompare[i]] <= attemptchecker[codecompare[i]]){
            whitetemp++;
          }
          if(attemptchecker[attempt[i]] <= codecomparechecker[attempt[i]]){
            whitetemp++;
          }
        }
        if(whitetemp - currentblackpegs > white_hits || whitetemp - currentblackpegs + 2*lengthfromend < white_hits){
          return false;
        }
      }
      return true;
    }
};


int main(){
    set_random_seed();
    int length, num;
    std::cout << std::endl;
    std::cout << "--------------- Welcome to the Mastermind Game Solver --------------" << std::endl;
    std::cout << "This program generates a random hidden code and attempts to solve it" << std::endl;
    std::cout << "as efficiently and quickly as possible" << std::endl;
    std::cout << std::endl;
    std::cout << "Please enter the length of the code:" << std::endl;
    std::cin >> length;
    std::cout << "Please enter the number of possible values (or colors):" << std::endl;
    std::cin >> num;
    mm_solver solver;
    solver.init(length, num);
    mm_code_maker maker;
    maker.init(length, num);
    maker.generate_sequence();
    int black_hits=0, white_hits=0;
    int attempts_limit = 5000;
    int attempts = 0;
    while((black_hits < length) && (attempts < attempts_limit)){
        std::vector<int> attempt;
        solver.create_attempt(attempt);
        maker.give_feedback(attempt, black_hits, white_hits);
        std::cout << "attempt: " << std::endl;
        for(int i = 0; i < attempt.size(); i++){
            std::cout << attempt[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;
        solver.learn(attempt, black_hits, white_hits);
        attempts++;
    }
    if(black_hits == length){
        std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
    }
    else{
        std::cout << "after " << attempts << " attempts still no solution" << std::endl;
    }
    std::cout << "the sequence generated by the code maker was:" << std::endl;
    for(int i = 0; i < maker.sequence.size(); i++){
        std::cout << maker.sequence[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}

void set_random_seed(){
    std::srand(std::time(0));
}

int randn(int n){
    return std::rand() % n;
}
