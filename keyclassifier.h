#ifndef KEYCLASSIFIER_H
#define KEYCLASSIFIER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "toneprofiles.h"
#include "preferences.h"

class KeyClassifier{
public:
	KeyClassifier(int);
	~KeyClassifier();
	int classify(const std::vector<double>&, const Preferences&);
private:
	ToneProfile* major;
	ToneProfile* minor;
};

#endif