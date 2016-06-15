#include "star.h"
#include "stargroup.h"
#include "probe.h"
#include "prod.h"
#include "probe.h"
#include "collisions.h"
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <tablelib/table.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;


#define PI       3.1415926535
#define MINRANGE 0.1
#define MAXRANGE 0.167

std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

vector<int> get_list_sizes(vector< vector<Star> > lists) {
    int i;
    vector<int> result;

    for (i=0; i<lists.size(); i++) {
        result.push_back(lists[i].size());
    }

    return result;
}

bool safe_distance_from_center(Star star) {
  Point star_pt(star.x, star.y), origin(0, 0);
  
  double dist = distance(star_pt, origin);
  
  if (MINRANGE < dist && dist < MAXRANGE) {
    return true;
  } else {
    return false;
  }
}

// vector<Star> in_probe_range(vector<Star> stars, Probe probe) {
//   Point star_pt;
//   vector<Star> result;
//   int i;
// 
//   for (i=0; i<stars.size(); i++) {
//     star_pt = Point(stars[i].x, stars[i].y);
//     if (safe_distance_from_center(stars[i]) && (distance(star_pt, probe.center) < probe.radius)) {
//       result.push_back(stars[i]);
//     }
//   }
// 
//   return result;
// }

vector<Star> in_probe_range(vector<Star> stars, Probe probe) {
  Point star_pt;
  vector<Star> result;
  int i;

  for (i=0; i<stars.size(); i++) {
    star_pt = Point(stars[i].x, stars[i].y);
    if (safe_distance_from_center(stars[i]) && (probe.can_cover(stars[i]))) {
      result.push_back(stars[i]);
    }
  }

  return result;
}

vector< vector<Star> > get_probe_stars(vector<Star> stars, vector<Probe> probes) {
    int i;
    vector< vector<Star> > result;

    for (i=0; i<probes.size(); i++) {
        result.push_back(in_probe_range(stars, probes[i]));
    }

    return result;
}

vector<Star> apply_indices(vector< vector<Star> > probestars, vector<int> indices) {
    int i;
    vector<Star> result;

    for (i=0; i<indices.size(); i++) {
        result.push_back(probestars[i][indices[i]]);
    }

    return result;
}

vector<string> dimmer_gdr(string magpair) {
    vector<string> dimmers;
    ostringstream pair;

    vector<string> mags = split(magpair, ':');
    int wfs = stoi(mags[0]);
    int gdr = stoi(mags[1]);

    for (int i=gdr+1; i<=19; i++) {
        pair << wfs << ":" << i;
        dimmers.push_back(pair.str());
        pair.str(std::string());
    }

    return dimmers;
}

string get_wfs(string magpair) {
    vector<string> mags = split(magpair, ':');
    return mags[0];
}

string get_gdr(string magpair) {
    vector<string> mags = split(magpair, ':');
    return mags[1];
}

vector<string> dimmer_wfs(string magpair) {
    vector<string> dimmers;
    ostringstream pair;

    vector<string> mags = split(magpair, ':');
    int wfs = stoi(mags[0]);
    int gdr = stoi(mags[1]);

    for (int i=wfs+1; i<=19; i++) {
        pair << i << ":" << gdr;
        dimmers.push_back(pair.str());
        pair.str(std::string());
    }

    return dimmers;
}

void test_group(vector<Star> probestars, CombinationGenerator &stargroups, map<string, bool> &valid_mags) {
  string magpair;

  m.lock();
  StarGroup group = StarGroup(apply_indices(probestars, stargroups.next()));
  m.unlock();
  
  if ( group.valid() ) {
    if ( ! has_collisions(group, probes) ) {
      magpair = group.magpair();
      valid_mags[magpair] = true;

      for ( string dimmer_pair : dimmer_wfs(magpair) ) { valid_mags[dimmer_pair] = true; }
      for ( string dimmer_pair : dimmer_gdr(magpair) ) { valid_mags[dimmer_pair] = true; }
    }
  }
}

map<string, bool> get_valid_mags(vector< vector<Star> > probestars, vector<Probe> probes) {
    int i;
    string current_pair;
    map<string, bool> valid_mags_map;

    CombinationGenerator stargroups = CombinationGenerator(get_list_sizes(probestars));

    int count = 1;
    while (! stargroups.done) {
        thread thread1(test_group, &probestars, &stargroups, &valid_mags_map);
        thread thread1(test_group, &probestars, &stargroups, &valid_mags_map);
   }

    return valid_mags_map;
}

vector<Star> load_stars(string filename) {
    vector<Star> stars;
    vector<string> tokens;
    std::ifstream infile(filename);

    int current_line = 1;
    string line; getline(infile, line);
    for ( ; getline(infile, line); ) {
        if (current_line > 2) {
            tokens = split(line, '\t');
            double x = stod(tokens[0]);
            double y = stod(tokens[1]);
            double r = stod(tokens[18]);

            stars.push_back(Star(x, y, r));
        }
        current_line++;
    }

    return stars;
}

map<string, bool> valid_mags_in_starfield(vector<Star> stars, vector<Probe> probes) {
    return get_valid_mags(get_probe_stars(stars, probes), probes);
}

vector<string> files_in_dir(string dirname) {
    ostringstream path;
    vector<string> filenames;
    DIR *pDIR;
    struct dirent *entry;
    if( (pDIR = opendir(dirname.c_str())) != NULL ) {
        while( (entry = readdir(pDIR)) != NULL ) {
            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ) {
                path.str(std::string());
                path << dirname << entry->d_name;
                filenames.push_back(path.str());
            }
        }
        closedir(pDIR);
    }

    return filenames;
}

void dogrid_file(map<string, int> ValidMags, double nfiles) {
    cout << "grid\tconfig\twfsmag\tgdrmag\tprob\n";
    cout << "----\t------\t------\t------\t----\n";

    string wfs, gdr;
    double prob;
    for (auto const entry : ValidMags) {
        wfs = get_wfs(entry.first);
        gdr = get_gdr(entry.first);

        prob = entry.second / (nfiles * 1.0);

        cout << "grid\tdgnf\t" << wfs << "\t" << gdr << "\t" << prob << "\n";
    }
}

int main() {
    Point probe1_ctr(0.25, 0);
    Point probe2_ctr(0, 0.25);
    Point probe3_ctr(-0.25, 0);
    Point probe4_ctr(0, -0.25);

    double range_width = 0.4;

    Probe probe1(27, 94.5, 72);
    Probe probe2(121.5, 166.5, 144);
    Probe probe3(-27, -94.5, -72);
    Probe probe4(-121.5, -166.5, -144);

    Point origin(0, 0);

    vector<Probe> probes;
    probes.push_back(probe1);
    probes.push_back(probe2);
    probes.push_back(probe3);
    probes.push_back(probe4);

    vector<Star>  stars;

    map<string, int>  ValidMagnitudes;
    map<string, bool> CurrentFileValidMagnitudes;

    int count = 0;

    vector<string> starfield_files = files_in_dir("Bes2/");
    vector<string>::iterator curr_path;
    for (curr_path=starfield_files.begin(); curr_path!=starfield_files.end(); curr_path++) {
        if (count > 100) { break; }

        cerr << "Processing file " << *curr_path << endl;
        stars = load_stars(*curr_path);
        CurrentFileValidMagnitudes = valid_mags_in_starfield(stars, probes);

        for (auto const entry : CurrentFileValidMagnitudes) {
            if (ValidMagnitudes.count(entry.first) == 0) {
                ValidMagnitudes[entry.first] = 1;
            } else {
                ValidMagnitudes[entry.first]++;
            }
        }

        count++;
    }
    
    dogrid_file(ValidMagnitudes, count);

    return 0;
}
