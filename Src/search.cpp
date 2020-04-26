#include "search.h"

Search::Search() {
    //set defaults here
}

Search::~Search() {}

int min_int(int i, int j) {
    if (i > j) {
        return j;
    }
    return i;
}

int max_int(int i, int j) {
    if (i > j) {
        return i;
    }
    return j;
}

double get_f_value(const Node& input, const EnvironmentOptions& options) {
    return input.get_g() + options.heuristicweight * input.get_h();
}

std::pair<size_t, size_t> get_i_j(size_t in_hash, const int a) {
    return std::pair<size_t, size_t> (in_hash / size_t(a), in_hash % size_t(a));
}

size_t get_number(std::pair<size_t, size_t> ij, const int map_width) {
    return ij.first * size_t(map_width) + ij.second;
}

double return_H (const std::pair<int, int>& start,
                 const std::pair<int, int>& end,
                 const EnvironmentOptions& options) {
    if (options.searchtype == CN_SP_ST_DIJK) {
        return 0;
    }
    if (options.metrictype == CN_SP_MT_DIAG) {
        auto i = abs(start.first - end.first);
        auto j = abs(start.second - end.second);
        return (max_int(i, j) - min_int(i, j)) + CN_SQRT_TWO * min_int(i, j);
    }
    if (options.metrictype == CN_SP_MT_MANH) {
        return abs(start.first - end.first) + abs(start.second - end.second);
    }
    if (options.metrictype == CN_SP_MT_EUCL) {
        return sqrt((start.first - end.first) * (start.first - end.first) +
                    (start.second - end.second) * (start.second - end.second));
    }
    if (options.metrictype == CN_SP_MT_CHEB) {
        return max_int(abs(start.first - end.first), abs(start.second - end.second));
    }
    return 0;
}

bool allow_move_to_i_j(int s_i, int s_j, int i, int j,
                       const Map& map, const EnvironmentOptions& options) {
    if (map.getValue(s_i + i, s_j + j) != 0) {
        return false;
    }
    if (i == 0 && j == 0) {
        return false;
    }
    if (abs(i) + abs(j) == 2) {
        if (options.allowdiagonal == true) {
            int count_near = 0;
            if (map.getValue(s_i, s_j + j) == 1) {
                count_near += 1;
            }
            if (map.getValue(s_i + i, s_j) == 1) {
                count_near += 1;
            }
            if (count_near == 0) {
                return true;
            }
            if (count_near == 1) {
                if (options.cutcorners == true) {
                    return true;
                } else {
                    return false;
                }
            }
            if (count_near == 2) {
                if (options.allowsqueeze == true) {
                    return true;
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
    }
    return true;
}

SearchResult Search::startSearch(const Map& map, const EnvironmentOptions& options, int loglevel) {
    std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
    std::vector<Node> Node_info((size_t(map.getMapWidth() * map.getMapHeight())));
    auto cmp = [&](size_t first, size_t second) {
        if (first == second) {
            return false;
        }
        double first_f = get_f_value(Node_info.at(first), options);
        double second_f = get_f_value(Node_info.at(second), options);
        double first_g = Node_info.at(first).get_g();
        double second_g = Node_info.at(second).get_g();
        if (std::abs(first_f - second_f) < std::numeric_limits<double>::epsilon()) {
            if (std::abs(first_g - second_g) < std::numeric_limits<double>::epsilon()) {
                return first < second;
            }
            if (options.breakingties == CN_SP_BT_GMAX && first_g > second_g) {
                return true;
            }
            if (options.breakingties == CN_SP_BT_GMIN && first_g < second_g) {
                return true;
            }
        } else {
            if (first_f < second_f) {
                return true;
            }
        }
        return false;
    };
    std::set<size_t, decltype(cmp)> open_nodes(cmp);
    std::unordered_set<size_t> closed_nodes = {};
    size_t start_number = get_number(map.getMapStart(), map.getMapWidth());
    sresult.first_Node = start_number;
    size_t finish_number = get_number(map.getMapFinish(), map.getMapWidth());
    Node start;
    start.cord = map.getMapStart();
    start.g = 0;
    start.h = return_H(map.getMapStart(), map.getMapFinish(), options);
    Node_info[start_number] = start;
    open_nodes.insert(start_number);
    unsigned long number_of_steps = 0;
    while (!open_nodes.empty()) {
        number_of_steps += 1;
        std::pair<size_t, size_t> min_node_ij = get_i_j(*(open_nodes.begin()), map.getMapWidth());
        size_t min_node_number = get_number(min_node_ij, map.getMapWidth());
        open_nodes.erase(min_node_number);
        closed_nodes.insert(min_node_number);
        if (min_node_number == finish_number) {
            sresult.pathfound = true;
            sresult.numberofsteps = number_of_steps;
            sresult.nodescreated = open_nodes.size() + closed_nodes.size();
            sresult.pathlength = float(Node_info[min_node_number].g);
            Node cur_node = Node_info[min_node_number];
            std::list<Node> lresult;
            lresult.push_front(cur_node);
            while (cur_node.cord != map.getMapStart()) {
                cur_node = Node_info[cur_node.parent_node];
                lresult.push_front(cur_node);
            }
            std::chrono::time_point<std::chrono::system_clock> stop_time = std::chrono::system_clock::now();
            std::list<Node> hresult = {};
            int i_was = 0, j_was = 0, i_move = 0, j_move = 0;
            bool first = true, second = true;
            Node before;
            for (Node now_node : lresult) {
                if (first == true) {
                    first = false;
                    hresult = {now_node};
                } else {
                    if (second == true) {
                        i_move = now_node.get_i() - i_was;
                        j_move = now_node.get_j() - j_was;
                        second = false;
                    } else {
                        if (!(now_node.get_i() - i_was == i_move && now_node.get_j() - j_was == j_move)) {
                            hresult.push_back(before);
                            i_move = now_node.get_i() - i_was;
                            j_move = now_node.get_j() - j_was;
                        }
                    }
                }
                before = now_node;
                i_was = now_node.get_i();
                j_was = now_node.get_j();
            }
            hresult.push_back(before);
            sresult.lppath = lresult;
            sresult.hppath = hresult;
            std::chrono::duration<double> time_of_work = stop_time - start_time;
            sresult.time = time_of_work.count();
            if (loglevel == 1 || loglevel == 2) {
                std::set<size_t> open_out = {};
                for (size_t number : open_nodes) {
                    open_out.insert(number);
                }
                std::set<size_t> closed_out = {};
                for (size_t number : closed_nodes) {
                    closed_out.insert(number);
                }
                sresult.open_close_info.push_back(std::make_pair(open_out, closed_out));
                sresult.Node_info.push_back(Node_info);
            }
            sresult.hw = options.heuristicweight;
            sresult.map_size = size_t(map.getMapWidth());
            return sresult;
        } else {
            int min_node_i = int(min_node_ij.first);
            int min_node_j = int(min_node_ij.second);
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (allow_move_to_i_j(min_node_i, min_node_j, i, j, map, options) == true) {
                        Node new_top;
                        new_top.cord = std::pair<size_t, size_t> (min_node_i + i, min_node_j + j);
                        if (abs(i) + abs(j) == 2) {
                            new_top.g = Node_info[min_node_number].g + CN_SQRT_TWO;
                        } else {
                            new_top.g = Node_info[min_node_number].g + 1;
                        }
                        new_top.h = return_H(new_top.cord, map.getMapFinish(), options);
                        new_top.parent_node = min_node_number;
                        size_t new_top_cord = get_number(new_top.cord, map.getMapWidth());
                        if (closed_nodes.count(new_top_cord) == 0) {
                            if (open_nodes.count(new_top_cord) == 0) {
                                Node_info[new_top_cord] = new_top;
                                open_nodes.insert(new_top_cord);
                            } else {
                                if (new_top.g <= Node_info[new_top_cord].g) {
                                    open_nodes.erase(new_top_cord);
                                    Node_info[new_top_cord] = new_top;
                                    open_nodes.insert(new_top_cord);
                                }
                            }
                        }
                    }
                }
            }
            if (loglevel == 2) {
                std::set<size_t> open_out = {};
                for (size_t number : open_nodes) {
                    open_out.insert(number);
                }
                std::set<size_t> closed_out = {};
                for (size_t number : closed_nodes) {
                    closed_out.insert(number);
                }
                sresult.open_close_info.push_back(std::make_pair(open_out, closed_out));
                sresult.Node_info.push_back(Node_info);
            }
        }
        /* End of search */
    }
    sresult.pathfound = false;
    return sresult;
}

/*void Search::makePrimaryPath(Node curNode)
{
    //need to implement
}*/

/*void Search::makeSecondaryPath()
{
    //need to implement
}*/
