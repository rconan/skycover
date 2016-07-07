#include "star.h"
#include "stargroup.h"
#include "probe.h"
#include "prod.h"
#include "probe.h"
#include "collisions.h"
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <regex>
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

vector<Star> in_probe_range(vector<Star> stars, Probe probe, double probe_angle) {
  vector<Star> result;
  int i;

  for (i=0; i<stars.size(); i++) {
    // if (probe.name.compare("probe4") == 0) {
    //   cout << probe.name << " ";
    //   cout << stars[i].x << ", " << stars[i].y << " ";
    //   cout << "safe_distance: " << safe_distance_from_center(stars[i]) << " ";
    //   cout << "can_cover: " << probe.coverable_area.point_in_poly(stars[i].point()) << endl;
    // }

    if (safe_distance_from_center(stars[i]) && probe.can_cover(stars[i])) {
      stars[i].cablemax = (probe_angle + 90 + stars[i].bear) / 2;
      stars[i].cablemin = min(probe_angle, stars[i].bear);
      result.push_back(stars[i]);
    }
  }

  return result;
}

int partition(vector<Star>& stars, int p, int q)
{
    Star x = stars[p];
    int i = p;
    int j;

    for(j=p+1; j<q; j++)
    {
        if(stars[j].r <= x.r)
        {
            i = i+1;
            swap(stars[i], stars[j]);
        }

    }

    swap(stars[i], stars[p]);

    return i;
}

// Implementation of quicksort on Star objects. Sorts by magnitude.
void star_sort(vector<Star>& stars, int p, int q)
{
    int r;
    if(p < q)
    {
        r = partition(stars, p, q);
        star_sort(stars, p, r);  
        star_sort(stars, r+1, q);
    }
}

vector< vector<Star> > get_probe_stars(vector<Star> stars, vector<Probe> probes) {
    int i;
    vector< vector<Star> > result;

    vector<double> probe = { 72, 144, -144, -72 };

    for (i=0; i<probes.size(); i++) {
      result.push_back(in_probe_range(stars, probes[i], probe[i]));
    }

    for (i=0; i<result.size(); i++) {
      star_sort(result[i], 0, result[i].size());
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

vector<string> dimmer_pairs(string magpair) {
    vector<string> dimmers;
    ostringstream pair;

    vector<string> mags = split(magpair, ':');
    int wfs = stoi(mags[0]);
    int gdr = stoi(mags[1]);

    for (int i=wfs; i<=19; i++) {
      for (int j=gdr; j<=19; j++) {
        pair << i << ":" << j;
        dimmers.push_back(pair.str());
        pair.str(std::string());
      }
    }

    return dimmers;
}

bool is_valid_pair(vector< vector<Star> > probestars, vector<Probe> probes, double wfsmag, double gdrmag, int printflg) {
    int i;
    string current_pair;
    StarGroup current_group;
    map<string, bool> valid_mags_map;

    CombinationGenerator stargroups = CombinationGenerator(get_list_sizes(probestars));

    while (! stargroups.done) {
      current_group = StarGroup(apply_indices(probestars, stargroups.next()));

      if ( current_group.valid(wfsmag, gdrmag, 0) ) {
        // if (printflg) { transform_and_print(current_group, probes); }
        if ( ! has_collisions(current_group, probes) ) {
          // if (printflg) { transform_and_print(current_group, probes); }
          // transform_and_print(current_group, probes);
          // for (Star s : current_group.stars) {
          //   cout << s.x << " " << s.y << " " << s.r << endl;
          // }
          return true;
        }
      }
    }

    return false;
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
            double bear = stod(tokens[3]);

            smatch match;
            regex bear_regex("([0-9]+):([0-9]+):([0-9]+)");
            if (regex_match(tokens[3], match, bear_regex)) {
              // cout << "translating " << tokens[3] << " into ";
              int degrees = stoi(match.str(1));
              int minutes = stoi(match.str(2));
              double seconds = stod(match.str(3));
              bear = degrees + ( (minutes + (seconds / 60)) / 60);
              cout << bear << endl;
            }

            stars.push_back(Star(x, y, r, bear));
        }
        current_line++;
    }

    return stars;
}

vector< vector<Star> > probestars_in_bin(vector< vector<Star> > probestars, int bin) {
  vector< vector<Star> > result;
  vector<Star> probeX_stars;

  for ( vector<Star> stars : probestars ) {
    probeX_stars.clear();
    for ( Star s : stars ) {
      if (s.r <= bin) {
        probeX_stars.push_back(s);
      }
    }
    result.push_back(probeX_stars);
  }

  return result;
}

map<string, bool> valid_mags_in_starfield(vector<Star> stars, vector<Probe> probes) {
  int bin;
  ostringstream currentpairstream;
  string currentpair;
  vector< vector<Star> > probestars, current_bin;
  map<string, bool> result;

  probestars = get_probe_stars(stars, probes);

  for (int wfsmag=13; wfsmag<=19; wfsmag++) {
    for (int gdrmag=13; gdrmag<=19; gdrmag++) {
      currentpairstream.str(std::string());
      bin = max(wfsmag, gdrmag);

      currentpairstream << wfsmag << ":" << gdrmag;
      currentpair = currentpairstream.str();

      current_bin = probestars_in_bin(probestars, bin);

      // for ( Star s : current_bin[3] ) {
      //   cout << s.x << " " << s.y << " " << s.r << endl;
      // }

      // if (wfsmag == 13 && gdrmag == 14) {
      //   cout << "bins at 13 14: " << bin << endl;
      //   for (int k=0; k<4; k++) {
      //     cout << "current_bin[" << k << "]" << endl;
      //     for (Star s : current_bin[k]) {
      //       cout << s.x << " " << s.y << endl;
      //     }
      //   }
      // }

      if (result[currentpair] == true) { continue; }

      // result[currentpair] = is_valid_pair(current_bin, probes, wfsmag, gdrmag);

      if ( result[currentpair] == true ) {
        for ( string dimmerpair : dimmer_pairs(currentpair) ) { result[dimmerpair] = true; }
      }
    }
  }

  return result;
}

vector<string> files_in_dir(string dirname, string regexp) {
  ostringstream path;
  vector<string> filenames;
  DIR *pDIR;
  struct dirent *entry;
  if( (pDIR = opendir(dirname.c_str())) != NULL ) {
    while( (entry = readdir(pDIR)) != NULL ) {
      if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ) {
        if (regex_match(entry->d_name, regex(regexp))) {
          path.str(std::string());
          path << dirname << entry->d_name;
          filenames.push_back(path.str());
        }
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

int main(int argc, char *argv[]) {
    Probe probe4("probe4", -27, -94.5, -72);
    Probe probe3("probe3", -121.5, -166.5, -144);
    Probe probe2("probe2", 121.5, 166.5, 144);
    Probe probe1("probe1", 27, 94.5, 72);

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
    int wfsmag, gdrmag, nfiles;

    string file_regex;
    if (argc < 2) {
      file_regex = ".*";
    } else if (argc == 4) {
      wfsmag = atoi(argv[1]);
      gdrmag = atoi(argv[2]);
      nfiles = atoi(argv[3]);
    } else {
      file_regex = argv[1];
    }

    vector<string> starfield_files = files_in_dir("Bes/", ".*");
    vector<string>::iterator curr_path;

    bool valid_file;
    vector< vector<Star> > probestars, current_bin;

    int printflg = 0;
    double valid_files = 0;
    for (int i=0; i<nfiles; i++) {
      // cerr << "Processing file " << starfield_files[i] << endl;
      stars = load_stars(starfield_files[i]);

      probestars  = get_probe_stars(stars, probes);
      current_bin = probestars_in_bin(probestars, max(wfsmag, gdrmag));

      // for (int k=0; k<4; k++) {
      //   cout << "current_bin[" << k << "]" << endl;
      //   for (Star s : current_bin[k]) {
      //     cout << s.x << " " << s.y << endl;
      //   }
      // }
      // cout << endl;

      if (i == 19) { printflg = 1; }

      if (is_valid_pair(current_bin, probes, wfsmag, gdrmag, printflg)) { valid_files++; }
      printflg = 0;
    }

    cerr << valid_files / nfiles << endl;

    /**
    for (curr_path=starfield_files.begin(); curr_path!=starfield_files.end(); curr_path++) {
      cerr << "Processing file " << *curr_path << endl;
      stars = load_stars(*curr_path);
      CurrentFileValidMagnitudes = valid_mags_in_starfield(stars, probes);

      for (auto const entry : CurrentFileValidMagnitudes) {
        if ( entry.second == true ) {
          if (ValidMagnitudes.count(entry.first) == 0) {
            ValidMagnitudes[entry.first] = 1;
          } else {
            ValidMagnitudes[entry.first]++;
          }
        }
      }

      count++;
    }
    **/


    // for ( auto const entry : ValidMagnitudes ) {
    //   cout << entry.first << ": " << entry.second << endl;
    // }
    
    // dogrid_file(ValidMagnitudes, count);

    return 0;
}
