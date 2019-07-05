/*	Grand Programming Challenge
*	25.11.2016
*	autor: Tibor Galko
*/


#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <iterator>

using namespace std;


class Globals {
private:
	map<string, vector<int>*>* dbWordMapGlobal;
public:
	map<string, vector<int>*>* Set_dbWordMap(map<string, vector<int>*>* dbWordMap) {
		dbWordMapGlobal = dbWordMap;
		return dbWordMapGlobal;
	}
	map<string, vector<int>*>* Get_dbWordMap() {
		return dbWordMapGlobal;
	}
};

Globals word_map;

list<int>* find_keywords(list <string> *keyword_list) {

	list<string>::iterator keyword_it_front, keyword_it_back;

	keyword_it_front = keyword_list->begin();
	keyword_it_back = keyword_list->end();

	map<string, vector<int>*> *wordMap;
	wordMap = word_map.Get_dbWordMap();

	vector<int>* lineVector;
	vector<vector<int>*> vectors;
	list<int>* result;
	result = new list<int>;
	map < string, vector<int>*>::iterator it_word_map;

	//pozbierat vektory z wordmap
	while (keyword_it_front != keyword_it_back) {
		it_word_map = (*wordMap).find((*keyword_it_front));
		if (it_word_map != wordMap->end()) {	//ak sa slovo nenajde vo wordmap preskoci sa
			lineVector = it_word_map->second;
			vectors.push_back(lineVector);	//pridanie vectora
			keyword_it_front++;
		}
		else keyword_it_front++;
	}
	if (vectors.empty()) return NULL;

	vector<vector<int>*>::iterator vectors_front, vectors_back;
	vectors_front = vectors.begin();
	vectors_back = vectors.end();

	//sortovanie vectorov
	while (vectors_front != vectors_back) {
		sort((*vectors_front)->begin(), (*vectors_front)->end());
		vectors_front++;
	}
	vectors_front = vectors.begin();
	vectors_back = vectors.end();

	if (vectors.size() == 1) {	//pre jeden keyword
		(*vectors_front)->erase(unique((*vectors_front)->begin(), (*vectors_front)->end()), (*vectors_front)->end());	//vymazanie opakujucich sa riadkov
		copy((*vectors_front)->begin(), (*vectors_front)->end(), back_inserter(*result));
	}
	else {	//pre viac keywordov
		vector<int> int_vector, temp_vector;
		temp_vector = *(*vectors_front);
		vectors_front++;

		while (vectors_front != vectors_back) {
			set_intersection(temp_vector.begin(), temp_vector.end(), (*vectors_front)->begin(), (*vectors_front)->end(), back_inserter(int_vector));
			temp_vector = int_vector;
			int_vector.clear();
			vectors_front++;
		}
		if (temp_vector.empty()) return NULL;
		else {
			temp_vector.erase(unique(temp_vector.begin(), temp_vector.end()), temp_vector.end());	//vymazanie opakujucich sa riadkov
		}
		copy(temp_vector.begin(), temp_vector.end(), back_inserter(*result));
	}
	return result;
}


//g++ GrandChallenge.cpp -o GrandChallenge -Wall -std=c++11
int main() {

	cout << "Vyhladavac z IMDb databazy 5000 filmov\n" << endl;

	ifstream ifs("movie_metadata.csv", ios::in);	//otvorenie datasheetu s modom citania
	if (!ifs.is_open()) {	//kontrola otvorenia
		cerr << "Chyba. Subor movie_metadata.csv sa nepodarilo najst." << endl;	//chybovy vypis a koniec
		return 1;
	}

	int lineCounter = 1;
	string line;
	map<int, string> dbLineMap;	//obsahuje riadok a cislo riadku
	map<int, string>::iterator lineItFront, lineItBack;
	while (getline(ifs, line)) {

		if (lineCounter != 1) {	//prvy riadok obsahujuci premenne v databaze sa vynecha
			dbLineMap[lineCounter++] = line;
		}
		else lineCounter++;
	}

	int cnt = 1;	//pouzivanie na pocitanie stlpcov
	stringstream lineStream, wordstream, plotstream;

	map<string, vector<int>*> dbWordMap;
	vector<int>* lineNumVector;
	lineItFront = dbLineMap.begin();
	lineItBack = dbLineMap.end();

	/*parsovanie databazy na jednotlive slova*/
	while (lineItFront != lineItBack) {

		lineStream.clear();
		lineStream.str("");
		lineStream << (*lineItFront).second;	//nacita riadok do streamu

		string word;
		while (getline(lineStream, word, ',')) {	//rozdeli riadok delimiter je ciarka

			transform(word.begin(), word.end(), word.begin(), ::tolower);	//vo wordlist iba same male pismena

			if (cnt == 1) {	//pridanie color
				wordstream.clear();
				wordstream.str("");
				wordstream << word;
				while (getline(wordstream, word, ' ')) {
					try {
						lineNumVector = dbWordMap.at(word);	//ak uz slovo vo word map je tak sa prida iba dalsie cislo riadku do vectora
						lineNumVector->push_back((*lineItFront).first);
					}
					catch (const out_of_range& oor) {
						lineNumVector = new vector<int>();
						lineNumVector->push_back((*lineItFront).first);
						dbWordMap[word] = lineNumVector;
					}
				}
				cnt++;
			}
			else if (cnt == 7 || cnt == 11 || cnt == 12 || cnt == 15) {	//rozdelovanie actor names a movie title
				wordstream.clear();
				wordstream.str("");
				wordstream << word;	//nacita riadok do streamu
				while (getline(wordstream, word, ' ')) {
					if (cnt == 12 && word.find("Â") != string::npos) {	//orezanie bugu za nazvami
						word.erase(word.size() - 2);
					}
					try {
						lineNumVector = dbWordMap.at(word);	//ak uz slovo vo word map je tak sa prida iba dalsie cislo riadku do vectora
						lineNumVector->push_back((*lineItFront).first);
					}
					catch (const out_of_range& oor) {
						lineNumVector = new vector<int>();
						lineNumVector->push_back((*lineItFront).first);
						dbWordMap[word] = lineNumVector;
					}
				}
				cnt++;
			}
			else if (cnt == 17 || cnt == 10) {	//rozdelovanie plot keywords a genres
				wordstream.clear();
				wordstream.str("");
				wordstream << word;	//nacita riadok do streamu
				while (getline(wordstream, word, '|')) {
					plotstream.clear();
					plotstream.str("");
					plotstream << word;
					while (getline(plotstream, word, ' ')) {
						try {
							lineNumVector = dbWordMap.at(word);	//ak uz slovo vo word map je tak sa prida iba dalsie cislo riadku do vectora
							lineNumVector->push_back((*lineItFront).first);
						}
						catch (const out_of_range& oor) {
							lineNumVector = new vector<int>();
							lineNumVector->push_back((*lineItFront).first);
							dbWordMap[word] = lineNumVector;
						}
					}
				}
				cnt++;
			}
			else cnt++;
		}
		cnt = 1;
		lineItFront++;
	}

	word_map.Set_dbWordMap(&dbWordMap);

	//Zaciatok hladania
	while (1) {
		cout << "Zadajte herca, zaner,... pre informacie o filme.\nZadajte n pre skoncenie" << endl;

		string userInput;

		getline(cin, userInput);	//nacita user input
		if (userInput == "n") break;

		transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);	//transformovanie stringu na vsetky male pismena

		istringstream split(userInput);
		list<string>* keyword_list;
		keyword_list = new list<string>();
		for (string keyword; getline(split, keyword, ' '); keyword_list->push_back(keyword)) {}	//rozdelenie input stringu na jednotlive slova delimiter medzera 
																								//a rozdelene slovo vlozene do keyword_list		
		list<int>* results;
		results = new list<int>();
		if ((results = find_keywords(keyword_list)) == NULL) {
			cout << "Nepodarilo sa najst zhodu" << endl;
		}
		else {
			int filmCounter = 0;
			list<int>::iterator results_it_Front, results_it_Back;
			results_it_Front = results->begin();
			results_it_Back = results->end();

			//vypisy
			while (results_it_Front != results_it_Back) {
				
				cout << dbLineMap[*results_it_Front] << endl << "film je: " << *results_it_Front << " v databaze"<< endl << endl;
				results_it_Front++;
				filmCounter++;
			}
			cout << "Pocet vysledkov: " << filmCounter << endl;
		}
	}

	ifs.close();
	delete lineNumVector;


	return 0;
}