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
}

bool collisions_while_tracking(vector<Probe> probes, StarGroup group) {
  for (int k=0; k<probes.size(); k++) {
    probes[k].current_star = group.stars[k];
    probes[k].base_star    = group.stars[k];
    probes[k].backward_transfer_idx = 0;
  }


    for (int i=0; i<=MINIMUMTRACK_DEG; i++) {
      for (int j=0; j<probes.size(); j++) {
        probes[j].track(i * (PI / 180));
        if (has_collisions_with_current_stars(probes)) {

          // print_with_current_stars(probes);
          // for (Probe p : probes) {
          //   p.current_star.point().print("g");
          // }

          return true;
        }
      }
    }

    return false;

  /**
  vector<int> list_sizes;
  for (Probe p : probes) {
    list_sizes.push_back(p.backward_transfers.size());
  }

  CombinationGenerator transfer_combos = CombinationGenerator(list_sizes);

  while (!transfer_combos.done) {
    vector<int> indices = transfer_combos.next();
    for (int i=0; i<probes.size(); i++) {
      probes[i].backward_transfer_idx = indices[i];
    }

    bool collisions = false;

    for (int i=0; i<=MINIMUMTRACK_DEG; i++) {
      for (int j=0; j<probes.size(); j++) {
        probes[j].track(i * (PI / 180));
        if (has_collisions_with_current_stars(probes)) {

          // print_with_current_stars(probes);
          // for (Probe p : probes) {
          //   p.current_star.point().print("g");
          // }

          collisions = true;
          break;
        }
      }
      if (collisions == true) {
        break;
      }
    }

    if (collisions == false) {
      return false;
    }
  }

  return true;
  **/
}

bool trackable(vector<Probe> probes, StarGroup group, int wfsmag, int gdrmag) {
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
      if (probes[j].track(i * (PI / 180)) == -1) {
        return false;
      }
    }
    // print_with_current_stars(probes);

    while (has_collisions_with_current_stars(probes)) {
      if (colliding_in_parts(probes[0].transform_parts(probes[0].current_star.point()),
                             probes[probes.size()-1].transform_parts(probes[probes.size()-1].current_star.point()))) {
        if (probes[probes.size()-1].backtrack(i * (PI / 180)) == -1) {
          nobacktrack = true;
          break;
        }
      }

      for (int k=0; k<probes.size()-1; k++) {
        // cerr << "checking probes " << k << " and " << (k+1) << endl;

        // if (k == 2) {
        //   probes[2].base_star.point().print("g");
        //   probes[2].current_star.point().print("g");
        //   cerr << endl;
        // }

        // cerr << "checking probes " << k << " and " << (k+1) << endl;
        if (colliding_in_parts(probes[k].transform_parts(probes[k].current_star.point()),
                               probes[k+1].transform_parts(probes[k+1].current_star.point()))) {
          // if (k == 2) {
          //   probes[k].current_star.point().print("g");
          //   probes[k+1].current_star.point().print("m");
          // }
          // cerr << "found collision between probes " << k << " and " << (k+1) << endl;

          if (probes[k].backtrack(i * (PI / 180)) == -1) {
            // cerr << "    couldn't resolve backtrack for probe " << k << endl;
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

/**
bool trackable(vector<Probe> probes, StarGroup group, int wfsmag, int gdrmag) {
  for (int k=0; k<probes.size(); k++) {
    probes[k].current_star = group.stars[k];
    probes[k].base_star    = group.stars[k];
    probes[k].backward_transfer_idx = 0;
  }

  for (int i=0; i<=MINIMUMTRACK_DEG; i++) {
    for (int l=0; l<probes.size(); l++) {
      if (probes[l].track(i * (PI / 180)) == -1) {
        return false;
      }
    }
  }

  for (int j=0; j<probes.size(); j++) {
    if (probes[j].track(MINIMUMTRACK) == -1) {
      return false;
    }
    if (probes[j].distance_tracked < MINIMUMTRACK) {
      return false;
    }
  }

    if (collisions_while_tracking(probes, group)) {
      return false;
    }

  return true;
}
**/

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

    while (has_collisions_with_current_stars(probes)) {
      if (colliding_in_parts(probes[0].transform_parts(probes[0].current_star.point()),
                             probes[probes.size()-1].transform_parts(probes[probes.size()-1].current_star.point()))) {
        if (probes[probes.size()-1].backtrack(i * (PI / 180)) == -1) {
          nobacktrack = true;
          break;
        }
      }

      for (int k=0; k<probes.size()-1; k++) {
        // cerr << "checking probes " << k << " and " << (k+1) << endl;

        // if (k == 2) {
        //   probes[2].base_star.point().print("g");
        //   probes[2].current_star.point().print("g");
        //   cerr << endl;
        // }

        // cerr << "checking probes " << k << " and " << (k+1) << endl;
        if (colliding_in_parts(probes[k].transform_parts(probes[k].current_star.point()),
                               probes[k+1].transform_parts(probes[k+1].current_star.point()))) {
          // if (k == 2) {
          //   probes[k].current_star.point().print("g");
          //   probes[k+1].current_star.point().print("m");
          // }
          // cerr << "found collision between probes " << k << " and " << (k+1) << endl;

          if (probes[k].backtrack(i * (PI / 180)) == -1) {
            // cerr << "    couldn't resolve backtrack for probe " << k << endl;
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

/**
void track_and_print_probes(vector<Probe> probes, StarGroup group) {
  for (int k=0; k<probes.size(); k++) {
    probes[k].current_star = group.stars[k];
    probes[k].base_star    = group.stars[k];
  }

  for (int i=0; i<=MINIMUMTRACK_DEG; i++) {
    for (int j=0; j<probes.size(); j++) {
      probes[j].track(i * (PI / 180));

      vector<Polygon> transformed_parts = probes[j].transform_parts(probes[j].current_star.point());

      transformed_parts[1].polyprint();
      transformed_parts[0].polyprint();
      transformed_parts[2].polyprint();
      probes[j].Base.polyprint();
    }
  }
}
**/

void here() {
  cout << "here" << endl;
}

bool is_valid_pair(vector<Star> stars, vector< vector<Star> > probestars, vector<Probe> probes, double wfsmag, double gdrmag, int printflg) {
    int i;
    string current_pair;
    StarGroup current_group;
    map<string, bool> valid_mags_map;

    CombinationGenerator stargroups = CombinationGenerator(get_list_sizes(probestars));

    vector< vector<Star> > transfers;
    vector<int> probes_with_transfers;

    int count = 0;

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

      
      if (current_group.valid(wfsmag, gdrmag, 0)) {

        // transform_and_print(probes, current_group);
        // exit(0);

        if ( !has_collisions_in_parts(current_group, probes) ) {

          // transform_and_print(probes, current_group);
          // return true;

          populate_backward_transfers(probes, current_group, stars, wfsmag, gdrmag);

          for (Probe p : probes) {
            if (p.needs_transfer && p.backward_transfers.size() == 0) {
              return false;
            }
          }

          if (trackable(probes, current_group, wfsmag, gdrmag)) {
            track_and_print_probes(probes, current_group);
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
              // cout << "translating " << tokens[3] << " into ";
              int degrees = stoi(match.str(1));
              int minutes = stoi(match.str(2));
              double seconds = stod(match.str(3));
              bear = degrees + ( (minutes + (seconds / 60)) / 60);
              // cout << bear << endl;
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

int main(int argc, char *argv[]) {
  Probe probe1(0);
  Probe probe2(90);
  Probe probe3(180);
  Probe probe4(-90);

  Point origin(0, 0);

  vector<Probe> probes;
  probes.push_back(probe1);
  probes.push_back(probe2);
  probes.push_back(probe3);
  probes.push_back(probe4);

  // vector<Star> stars_ = load_stars("Bes/bes.0006.cat");
  // probe3.backward_transfers = stars_;

  // Point _a(343.6, -213.8);
  // _a = _a.rotate(5 * (PI / 180));
  // Star a(_a.x, _a.y, 0, 0);

  // probe3.track(probe2.angle_to_point(_a.rotate(5 * (PI / 180))));

  // cerr << probe3.in_range(a) << endl;
  // probe3.axis.print("k");

  // exit(0);

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
    gdrmag = atoi(argv[2]);
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
    current_bin = probestars_in_bin(probestars, max(wfsmag, gdrmag));

    if (is_valid_pair(stars, current_bin, probes, wfsmag, gdrmag, 0)) {
      cerr << 1 << endl;
    } else {
      cerr << 0 << endl;
    }

    ostringstream starfile;
    write_stars(stars, "starfiles/starfield1.cat", wfsmag, gdrmag);
  } else {
    for (int i=0; i<nfiles; i++) {
      // cerr << "Processing file " << starfield_files[i] << ": ";
      stars = load_stars(starfield_files[i]);

      probestars  = get_probe_stars(stars, probes);
      current_bin = probestars_in_bin(probestars, max(wfsmag, gdrmag));

      if (is_valid_pair(stars, current_bin, probes, wfsmag, gdrmag, printflg)) {
        // cerr << starfield_files[i] << ": valid" << endl;
        valid_files++;
        ostringstream starfile;
        starfile << "starfiles/starfield" << valid_files << ".cat";
        write_stars(stars, starfile.str(), wfsmag, gdrmag);
      }
    }
  }
  cerr << valid_files / nfiles << endl;


  return 0;
}
