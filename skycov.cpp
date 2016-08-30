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


#define PI               3.1415926535
#define MINRANGE         (0.1   * 3600)
#define MAXRANGE         (0.167 * 3600)
#define MINIMUMTRACK     (60 * (PI / 180))
#define MINIMUMTRACK_DEG 60
#define USE_OBSCURATION  1
#define MAX_OBSCURED     1

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

vector<Star> in_probe_range(vector<Star> stars, Probe probe, double probe_angle) {
  vector<Star> result;
  int i;

  for (i=0; i<stars.size(); i++) {
    if (safe_distance_from_center(stars[i]) && probe.in_range(stars[i])) {
      stars[i].cablemax = (probe_angle + 90 + stars[i].bear) / 2;
      stars[i].cablemin = min(probe_angle, stars[i].bear);
      result.push_back(stars[i]);
    }
  }

  return result;
}

int partition(vector<Star>& stars, int p, int q)
{
  Point origin(0, 1);
    Star x = stars[p];
    int i = p;
    int j;

    for(j=p+1; j<q; j++)
    {
      if(angle_between_vectors(origin, stars[j].point()) >= angle_between_vectors(origin, x.point()))
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

vector<Star> stars_in_angular_distance(Point p, vector<Star> stars, double dist) {
  vector<Star> result;
  for (Star s : stars) {
    if (angle_between_vectors(p, s.point()) < dist) {
      result.push_back(s);
    }
  }

  star_sort(result, 0, result.size());

  return result;
}

Star find_backward_transfer(Probe probe, Star initial_star, vector<Star> stars) {
  Star transfer;
  double max_track = angle_between_vectors(initial_star.point(), probe.axis.rotate(90 * (PI / 180)));

  vector<Star> candidates = stars_in_angular_distance(probe.axis, stars, max_track);
  for (Star s : candidates) {
    if (s.r <= initial_star.r) {
      transfer = s;
    }
  }

  return transfer;
}

bool valid_with_current_stars(vector<Probe> probes, int wfsmag, int gdrmag) {
  vector<Star> stars;
  for (Probe p : probes) { stars.push_back(p.current_star); }
  StarGroup group(stars);

  return group.valid(wfsmag, gdrmag, 0);
}

void print_with_current_stars(vector<Probe> probes) {
  for (int i=0; i<probes.size(); i++) {
    vector<Polygon> transformed_parts = probes[i].transform_parts(probes[i].current_star.point());
    transformed_parts[1].polyprint();
    transformed_parts[0].polyprint();
    transformed_parts[2].polyprint();
    probes[i].Base.polyprint();
  }

  get_m3_obscuration(0).polyprint();
}


bool trackable(vector<Probe> probes, StarGroup group, int wfsmag, int gdrmag) {
  for (int k=0; k<probes.size(); k++) {
    probes[k].current_star = group.stars[k];
    probes[k].base_star    = group.stars[k];
    probes[k].backward_transfer_idx = 0;
  }

  bool nobacktrack = false;

  for (int i=0; i<MINIMUMTRACK_DEG; i++) {
    for (int j=0; j<probes.size(); j++) {
      probes[j].track(i * (PI / 180));
      probes[j].used_transfers.clear();
      if (probes[j].track(i * (PI / 180)) == -1) {
        return false;
      }
    }


    while (has_collisions_with_current_stars(probes, USE_OBSCURATION)) {
      if (colliding_in_parts(probes[0].transform_parts(probes[0].current_star.point()),
                             probes[probes.size()-1].transform_parts(probes[probes.size()-1].current_star.point()))) {
        if (probes[probes.size()-1].backtrack(i * (PI / 180)) == -1) {
          nobacktrack = true;
          break;
        }
      }

      for (int k=0; k<probes.size()-1; k++) {
        if (colliding_in_parts(probes[k].transform_parts(probes[k].current_star.point()),
                               probes[k+1].transform_parts(probes[k+1].current_star.point()))) {
          if (probes[k].backtrack(i * (PI / 180)) == -1) {
            nobacktrack = true;
            break;
          }
        }
      }

      for (int l=0; l<probes.size(); l++) {
        if (obscured(probes[l].current_star)) {
          if (probes[l].backtrack(i * (PI / 180)) == -1) {
            nobacktrack = true;
            break;
          }
        }
      }

      if (nobacktrack) { break; }
    }
  }

  if (nobacktrack) {
    return false;
  } else {
    return true;
  }
}

void populate_backward_transfers(vector<Probe> &probes, StarGroup group, vector<Star> stars, int wfsmag, int gdrmag) {
  for (int i=0; i<probes.size(); i++) {
    double track_dist = probes[i].track_distance(group.stars[i]);
    if (track_dist < MINIMUMTRACK) {
      probes[i].needs_transfer = true;
      probes[i].get_backward_transfers(stars, track_dist, max(wfsmag, gdrmag));
    }
  }
}

void transform_and_print(vector<Probe> probes, StarGroup group) {
  for (int i=0; i<probes.size(); i++) {
    // group.stars[i].point().print("red");
    
    vector<Polygon> transformed_parts = probes[i].transform_parts(group.stars[i].point());
    transformed_parts[1].polyprint();
    transformed_parts[0].polyprint();
    transformed_parts[2].polyprint();
    probes[i].Base.polyprint();
  }

  get_m3_obscuration(0).polyprint();
}

void track_and_print_probes(vector<Probe> probes, StarGroup group) {
  for (int k=0; k<probes.size(); k++) {
    probes[k].current_star = group.stars[k];
    probes[k].base_star    = group.stars[k];
    probes[k].backward_transfer_idx = 0;
  }

  bool nobacktrack = false;

  for (int i=0; i<MINIMUMTRACK_DEG; i++) {
    // probes[2].base_star.point().print("g");
    // probes[2].current_star.point().print("g");
    for (int j=0; j<probes.size(); j++) {
      probes[j].track(i * (PI / 180));
      probes[j].used_transfers.clear();
    }
    // print_with_current_stars(probes);

    while (has_collisions_with_current_stars(probes, USE_OBSCURATION)) {
      if (colliding_in_parts(probes[0].transform_parts(probes[0].current_star.point()),
                             probes[probes.size()-1].transform_parts(probes[probes.size()-1].current_star.point()))) {
        if (probes[probes.size()-1].backtrack(i * (PI / 180)) == -1) {
          nobacktrack = true;
          break;
        }
      }

      for (int k=0; k<probes.size()-1; k++) {

        if (colliding_in_parts(probes[k].transform_parts(probes[k].current_star.point()),
                               probes[k+1].transform_parts(probes[k+1].current_star.point()))) {

          if (probes[k].backtrack(i * (PI / 180)) == -1) {
            nobacktrack = true;
            break;
          }
        }
      }

      for (int l=0; l<probes.size(); l++) {
        if (obscured(probes[l].current_star)) {
          if (probes[l].backtrack(i * (PI / 180)) == -1) {
            nobacktrack = true;
            break;
          }
        }
      }

      if (nobacktrack) { break; }
    }

    print_with_current_stars(probes);
  }
}

void here() {
  cout << "here" << endl;
}

int has_empty_bin(vector< vector<Star> > bins) {
  int emptybins = 0;
  for (vector<Star> bin : bins) {
    if (bin.size() == 0) {
      emptybins++;
    }
  }

  return emptybins;
}

int get_empty_idx(vector< vector<Star> > bins) {
  int idx = -1;
  for (int i=0; i<bins.size(); i++) {
    if (bins[i].size() == 0) {
      idx = i;
      break;
    }
  }

  return idx;
}

bool is_valid_pair(vector<Star> stars, vector< vector<Star> > probestars, vector<Probe> probes, double wfsmag, double gdrmag, int printflg) {
    int i;
    string current_pair;
    StarGroup current_group;
    map<string, bool> valid_mags_map;

    for (int i=0; i<probestars.size(); i++) {
      probestars[i].push_back(probes[i].default_star);
    }

    CombinationGenerator stargroups = CombinationGenerator(get_list_sizes(probestars));

    vector< vector<Star> > transfers;
    vector<int> probes_with_transfers;

    int count = 0;

    // cerr << "already done: " << stargroups.done << endl;

    while ( !stargroups.done ) {
      transfers.clear();
      probes_with_transfers.clear();
      for (Probe p : probes) {
        p.needs_transfer = false;
      }

      current_group = StarGroup(apply_indices(probestars, stargroups.next()));

      for (int i=0; i<probes.size(); i++) {
        probes[i].backward_transfers.clear();
        probes[i].current_star = current_group.stars[i];
      }

      // if (emptyidx != -1) {
      //   current_group.add_star(probes[emptyidx].default_star);
      // }

      // transform_and_print(probes, current_group);

      if (current_group.valid(wfsmag, gdrmag, 0)) {

        // current_group.stars[1].print();

        // transform_and_print(probes, current_group);
        // exit(0);

        if ( !has_collisions_in_parts(current_group, probes, USE_OBSCURATION, MAX_OBSCURED) ) {

          // transform_and_print(probes, current_group);
          return true;

          populate_backward_transfers(probes, current_group, stars, wfsmag, gdrmag);

          for (Probe p : probes) {
            if (p.needs_transfer && p.backward_transfers.size() == 0) {
              return false;
            }
          }


          if (trackable(probes, current_group, wfsmag, gdrmag)) {
            // track_and_print_probes(probes, current_group);
            return true;
          } else {
            return false;
          }
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
            double x = stod(tokens[0]) * 3600;
            double y = stod(tokens[1]) * 3600;
            double r = stod(tokens[18]);
            double bear = stod(tokens[3]);

            smatch match;
            regex bear_regex("([0-9]+):([0-9]+):([0-9]+)");
            if (regex_match(tokens[3], match, bear_regex)) {
              int degrees = stoi(match.str(1));
              int minutes = stoi(match.str(2));
              double seconds = stod(match.str(3));
              bear = degrees + ( (minutes + (seconds / 60)) / 60);
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

      if (result[currentpair] == true) { continue; }

      // result[currentpair] = is_valid_pair(stars, current_bin, probes, wfsmag, gdrmag);

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

void write_stars(vector<Star> stars, string filename, int wfsmag, int gdrmag) {
  std::ofstream fout;
  fout.open(filename);

  // cerr << "writing stars to " << filename << endl;

  for (Star s : stars) {
    if (s.r < max(wfsmag, gdrmag)) {
      fout << s.x << " " << s.y << endl;
    }
  }

  fout.close();
}

vector<Probe> probes_except(vector<Probe> probes, int idx) {
  vector<Probe> res;
  for (int i=0; i<probes.size(); i++) {
    if (i != idx) {
      res.push_back(probes[i]);
    }
  }

  return res;
}

vector< vector<Star> > filter_empty_bin(vector< vector<Star> > bin) {
  vector< vector<Star> > res;
  for (int i=0; i<bin.size(); i++) {
    if (bin[i].size() != 0) {
      res.push_back(bin[i]);
    }
  }

  return res;
}

int main(int argc, char *argv[]) {
  Probe probe1(0);
  Probe probe2(90);
  Probe probe3(180);
  Probe probe4(-90);

  Star probe1default(0, 1000, 20, 0);
  Star probe2default(-1000, 0, 20, 0);
  Star probe3default(0, -1000, 20, 0);
  Star probe4default(1000, 0, 20, 0);

  probe1.default_star = probe1default;
  probe2.default_star = probe2default;
  probe3.default_star = probe3default;
  probe4.default_star = probe4default;

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
  } else {
    wfsmag = atoi(argv[1]);
    gdrmag = atoi(argv[1]);
    nfiles = atoi(argv[3]);
  }

  vector<string> starfield_files = files_in_dir("Bes/", ".*");
  vector<string>::iterator curr_path;

  bool valid_file;
  vector< vector<Star> > probestars, current_bin;
    int printflg = 0;
    double valid_files = 0;

  if (strcmp("--only", argv[3]) == 0) {
    cerr << "Processing file " << starfield_files[stoi(argv[4])-1] << endl;
    stars = load_stars(starfield_files[stoi(argv[4])-1]);


    probestars = get_probe_stars(stars, probes);
    // for (Star s : probestars[1]) {
    //  s.print();
    // }
    // cerr << "binning" << endl;
    current_bin = probestars_in_bin(probestars, max(wfsmag, gdrmag));

    if (is_valid_pair(stars, current_bin, probes, wfsmag, gdrmag, 0)) {
      cerr << 1 << endl;
    } else {
      cerr << 0 << endl;
    }

    ostringstream starfile;
    write_stars(stars, "starfiles_m3/starfield1.cat", wfsmag, gdrmag);
  } else {
    for (int i=0; i<nfiles; i++) {
      // cerr << "Processing file " << starfield_files[i] << ": ";
      stars = load_stars(starfield_files[i]);

      probestars  = get_probe_stars(stars, probes);
      current_bin = probestars_in_bin(probestars, max(wfsmag, gdrmag));

      if (is_valid_pair(stars, current_bin, probes, wfsmag, gdrmag, printflg)) {
        valid_files++;
        ostringstream starfile;
        starfile << "starfiles_m3/starfield" << (i+1) << ".cat";
        write_stars(stars, starfile.str(), wfsmag, gdrmag);
      } else {
        ostringstream starfile;
        starfile << "starfiles_m3/starfield" << (i+1) << "_invalid.cat";
        write_stars(stars, starfile.str(), wfsmag, gdrmag);
      }
    }
  }

  cout << valid_files / nfiles << endl;


  return 0;
}
