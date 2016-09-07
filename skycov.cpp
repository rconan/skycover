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


#define PI                      3.1415926535
#define MINRANGE                (0.1   * 3600)
#define MAXRANGE                (0.167 * 3600)
#define MINIMUMTRACK            (60 * (PI / 180))
#define MINIMUMTRACK_DEG        60
#define N_OK_OBSCRD_FOR_PHASING 1
#define N_OK_OBSCRD_FOR_4PROBE  0

bool PRINT = false;

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

void print_with_current_stars(vector<Probe> probes, Polygon obscuration) {
  for (int i=0; i<probes.size(); i++) {
    vector<Polygon> transformed_parts = probes[i].transform_parts(probes[i].current_star.point());
    transformed_parts[1].polyprint();
    transformed_parts[0].polyprint();
    transformed_parts[2].polyprint();
  }

  if (!obscuration.points.empty()) {
    obscuration.polyprint();
  }
}


bool trackable(vector<Probe> probes, StarGroup group, int wfsmag, int gdrmag, Polygon obscuration) {
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


    while (has_collisions_with_current_stars(probes, obscuration)) {
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

      if (!obscuration.points.empty()) {
        for (int l=0; l<probes.size(); l++) {
          if (star_is_obscured(probes[l].current_star, obscuration)) {
            if (probes[l].backtrack(i * (PI / 180)) == -1) {
              nobacktrack = true;
              break;
            }
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

void transform_and_print(vector<Probe> probes, StarGroup group, Polygon obscuration) {
  for (int i=0; i<probes.size(); i++) {
    // group.stars[i].point().print("red");
    
    vector<Polygon> transformed_parts = probes[i].transform_parts(group.stars[i].point());
    for (Polygon part : transformed_parts) {
      part.polyprint();
    }
  }

  if (!obscuration.points.empty()) {
    obscuration.polyprint();
  }
}

void track_and_print_probes(vector<Probe> probes, StarGroup group, Polygon obscuration) {
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
    }

    while (has_collisions_with_current_stars(probes, obscuration)) {
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

      if (!obscuration.points.empty()) {
        for (int l=0; l<probes.size(); l++) {
          if (star_is_obscured(probes[l].current_star, obscuration)) {
            if (probes[l].backtrack(i * (PI / 180)) == -1) {
              nobacktrack = true;
              break;
            }
          }
        }
      }

      if (nobacktrack) { break; }
    }

    print_with_current_stars(probes, obscuration);
  }
}

bool is_valid_pair_notracking(vector<Star> stars, vector< vector<Star> > probestars, vector<Probe> probes,
                              double wfsmag, double gdrmag, Polygon obscuration) {
  CombinationGenerator stargroups(get_list_sizes(probestars));

  while ( !stargroups.done ) {
    StarGroup current_group = StarGroup(apply_indices(probestars, stargroups.next()));
    
    if (current_group.valid(wfsmag, gdrmag)) {
      if ( !has_collisions_in_parts(current_group, probes, obscuration, N_OK_OBSCRD_FOR_4PROBE) ) {
        if (PRINT) {
          transform_and_print(probes, current_group, obscuration); 
        }
        return true;
      }
    }
  }

  return false;
}

bool is_valid_pair_tracking(vector<Star> stars, vector< vector<Star> > probestars, vector<Probe> probes,
                            double wfsmag, double gdrmag, Polygon obscuration) {

    CombinationGenerator stargroups(get_list_sizes(probestars));
  
    while ( !stargroups.done ) {
      for (Probe p : probes) {
        p.needs_transfer = false;
      }

      StarGroup current_group = StarGroup(apply_indices(probestars, stargroups.next()));

      for (int i=0; i<probes.size(); i++) {
        probes[i].backward_transfers.clear();
        probes[i].current_star = current_group.stars[i];
      }


      if (current_group.valid(wfsmag, gdrmag)) {
        if ( !has_collisions_in_parts(current_group, probes, obscuration, N_OK_OBSCRD_FOR_4PROBE) ) {
          populate_backward_transfers(probes, current_group, stars, wfsmag, gdrmag);

          // add nobacktrack logic
          bool nobacktrack = false;
          for (Probe p : probes) {
            if (p.needs_transfer && p.backward_transfers.size() == 0) {
              nobacktrack = true;
              break;
            }
          }

          if (nobacktrack) {
            continue;
          }

          if (trackable(probes, current_group, wfsmag, gdrmag, obscuration)) {
            if (PRINT) {
              track_and_print_probes(probes, current_group, obscuration); 
            }
            return true;
          }
        }
      }
    }

    return false;
}

bool is_valid_phasing_mag(vector< vector<Star> > probestars, vector<Probe> probes, double maglim, Polygon M3) {
  for (int i=0; i<probestars.size(); i++) {
    probestars[i].push_back(probes[i].default_star);
  }

  CombinationGenerator stargroups(get_list_sizes(probestars));

  while ( !stargroups.done ) {
    StarGroup current_group = StarGroup(apply_indices(probestars, stargroups.next()));

    if (current_group.valid_for_phasing(maglim)) {
      if ( !has_collisions_in_parts(current_group, probes, M3, N_OK_OBSCRD_FOR_PHASING) ) {
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

void write_stars(vector<Star> stars, string filename, int wfsmag, int gdrmag) {
  std::ofstream fout;
  fout.open(filename);

  for (Star s : stars) {
    if (s.r < max(wfsmag, gdrmag)) {
      fout << s.x << " " << s.y << endl;
    }
  }

  fout.close();
}

int number_valid_phasing_files(vector<string> starfld_files, vector<Probe> probes,
                               int maglim, int nfiles, Polygon M3) {
  int valid_files = 0;

  for (int i=0; i<nfiles; i++) {
    vector<Star> stars = load_stars(starfld_files[i]);

    vector< vector<Star> > probestars  = get_probe_stars(stars, probes);
    vector< vector<Star> > current_bin = probestars_in_bin(probestars, maglim);

    if (is_valid_phasing_mag(current_bin, probes, maglim, M3)) {
      valid_files++;
      ostringstream starfile;
      starfile << "starfiles/starfield" << valid_files << ".cat";
      write_stars(stars, starfile.str(), maglim, maglim);
    } else {
      ostringstream starfile;
      starfile << "starfiles/starfield" << valid_files << "_invalid.cat";
      write_stars(stars, starfile.str(), maglim, maglim);
    }
  }
  
  return valid_files;
}

int number_valid_4probe_files(vector<string> starfld_files, vector<Probe> probes, int wfsmag, int gdrmag,
                              int nfiles, Polygon obscuration, bool tracking) {
  int valid_files = 0;

  bool (*is_valid_pair)(vector<Star> stars, vector< vector<Star> > probestars, vector<Probe> probes,
                        double wfsmag, double gdrmag, Polygon obscuration);

  is_valid_pair = is_valid_pair_notracking;
  if (tracking) {
    is_valid_pair = is_valid_pair_tracking;
  }
  
  for (int i=0; i<nfiles; i++) {
    vector<Star> stars = load_stars(starfld_files[i]);

    vector< vector<Star> > probestars  = get_probe_stars(stars, probes);
    vector< vector<Star> > current_bin = probestars_in_bin(probestars, max(wfsmag, gdrmag));

    if (is_valid_pair(stars, current_bin, probes, wfsmag, gdrmag, obscuration)) {
      valid_files++;
      ostringstream starfile;
      starfile << "starfiles/starfield" << valid_files << ".cat";
      write_stars(stars, starfile.str(), wfsmag, gdrmag);
    } else {
      ostringstream starfile;
      starfile << "starfiles/starfield" << valid_files << "_invalid.cat";
      write_stars(stars, starfile.str(), wfsmag, gdrmag);
    }
  }
  
  return valid_files;
}

Polygon get_m3_obscuration() {
  return load_poly("m3_obsc.txt");
}

Polygon get_gclef_obscuration() {
  return load_poly("gclef_obsc.txt");
}


int main(int argc, char *argv[]) {
  string slider_body_file  = "probe_slider_body.txt";
  string slider_shaft_file = "probe_slider_shaft.txt";
  string baffle_tube_file  = "probe_baffle_tube.txt";

  Probe probe1(0,   25, slider_body_file, slider_shaft_file, baffle_tube_file);
  Probe probe2(90,  25, slider_body_file, slider_shaft_file, baffle_tube_file);
  Probe probe3(180, 25, slider_body_file, slider_shaft_file, baffle_tube_file);
  Probe probe4(270, 25, slider_body_file, slider_shaft_file, baffle_tube_file);

  Star probe1default(0,     1000,  20, 0);
  Star probe2default(-1000, 0,     20, 0);
  Star probe3default(0,     -1000, 20, 0);
  Star probe4default(1000,  0,     20, 0);

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

  Polygon GCLEF = get_gclef_obscuration();
  Polygon M3    = get_m3_obscuration();
  Polygon DGNF;

  /**
  arg format:
      ./skycov <--4probe | --phasing> <--gclef | --m3 | --dgnf> <wfsmag> <gdrmag> <nfiles>

  argv[1] -> regular simulation or phasing
  argv[2] -> obscuration type. '--dgnf' for no obscuration
  argv[3] -> tracking or notracking
  argv[4] -> print configurations or not
  argv[5] -> wfsmag
  argv[6] -> gdrmag
  argv[7] -> number of files to test
  **/
  
  bool    phasing, tracking;
  Polygon obscuration;
  int     wfsmag, gdrmag, nfiles;

  if (argc < 8) {
    cout << "usage: ./skycov <--4probe | --phasing> <--gclef | --m3 | --dgnf> <--track | --notrack> <--print | --noprint> <wfsmag> <gdrmag> <nfiles>" << endl;
    return 0;
  } else {
    if (strcmp(argv[1], "--phasing") == 0) {
      phasing = true;
    } else {
      phasing = false;
    }

    if (strcmp(argv[2], "--gclef") == 0) {
      obscuration = GCLEF;
    } else if (strcmp(argv[2], "--m3") == 0) {
      obscuration = M3;
    } else {
      obscuration = DGNF;
    }

    if (strcmp(argv[3], "--track") == 0) {
      tracking = true;
    } else {
      tracking = false;
    }

    if (strcmp(argv[4], "--print") == 0) {
      PRINT = true;
    }

    wfsmag = atoi(argv[5]);
    gdrmag = atoi(argv[6]);
    nfiles = atoi(argv[7]);
  }

  vector<string> starfield_files = files_in_dir("Bes/", ".*");

  double valid_files = 0;
  if (phasing) {
    valid_files = number_valid_phasing_files(starfield_files, probes, wfsmag, nfiles, M3);
  } else {
    valid_files = number_valid_4probe_files(starfield_files, probes, wfsmag, gdrmag, nfiles, obscuration, tracking);
  }

  cerr << valid_files / nfiles << endl;

  return 0;
}
