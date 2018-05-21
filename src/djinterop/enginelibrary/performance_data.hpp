/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_HPP
#define DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "database.hpp"
#include "musical_key.hpp"
#include "pad_colour.hpp"

namespace djinterop {
namespace enginelibrary {

/**
 * The `nonexistent_performance_data` exception is thrown when a request is
 * made to look up performance data for a given track id in a given database,
 * but there is no such performance data stored.
 */
class nonexistent_performance_data : public std::invalid_argument
{
public:
	explicit nonexistent_performance_data(int track_id) noexcept :
		invalid_argument{"PerformanceData does not exist in database"},
		track_id_{track_id}
	{}
	virtual ~nonexistent_performance_data() = default;
	int track_id() const noexcept { return track_id_; }
private:
	int track_id_;
};

/**
 * The `corrupt_performance_data` exception is thrown when internal expectations
 * of the performance data is stored within a database are not met.
 */
class corrupt_performance_data : public std::logic_error
{
public:
    explicit corrupt_performance_data(int track_id) noexcept :
        logic_error{"PerformanceData is corrupted or of unknown format"},
        track_id_{track_id}
    {}
    explicit corrupt_performance_data(int track_id, const char *msg) noexcept :
        logic_error{msg},
        track_id_{track_id}
    {}
	virtual ~corrupt_performance_data() = default;
	int track_id() const noexcept { return track_id_; }
private:
	int track_id_;
};

/**
 * The `track_beat_grid` struct holds information about a beat grid.
 *
 * A beat grid is represented by two points (measured as a sample offset) in a
 * track, each with an associated beat number/index.
 *
 * By convention, the Engine Prime analyses tracks so that the first beat is
 * at index -4 (yes, negative!) and the last beat is the first beat past the
 * usable end of the track, which may not necessarily be aligned to the first
 * beat of a 4-beat bar.  Therefore, the sample offsets typically recorded by
 * Engine Prime do not usually lie within the actual track.  If you want to
 * normalise any `track_beat_grid` to this approach, use the
 * `normalise_beat_grid` method.
 */
struct track_beat_grid
{
    /**
     * \brief Default constructor
     */
    track_beat_grid() :
        first_beat_index{0},
        first_beat_sample_offset{0},
        last_beat_index{0},
        last_beat_sample_offset{0}
    {}

    /**
     * \brief Construct a `track_beat_grid` from explicit values
     */
    track_beat_grid(
            int first_beat_index, double first_beat_sample_offset,
            int last_beat_index, double last_beat_sample_offset) :
        first_beat_index{first_beat_index},
        first_beat_sample_offset{first_beat_sample_offset},
        last_beat_index{last_beat_index},
        last_beat_sample_offset{last_beat_sample_offset}
    {}

	int first_beat_index;
    double first_beat_sample_offset;
	int last_beat_index;
    double last_beat_sample_offset;
};

/**
 * The `track_hot_cue_point` struct represents a hot cue within a track.
 */
struct track_hot_cue_point
{
    /**
     * \brief Default constructor
     */
    track_hot_cue_point() :
        is_set{false},
        label{},
        sample_offset{-1},
        colour{}
    {}

    /**
     * \brief Construct a `track_hot_cue_point` from explicit field values
     */
    track_hot_cue_point(
            bool is_set, const std::string &label,
            double sample_offset, const pad_colour &colour) :
        is_set{is_set},
        label{label},
        sample_offset{sample_offset},
        colour{colour}
    {}

    bool is_set;
    std::string label;
    double sample_offset;
    pad_colour colour;
};

/**
 * The `track_loop` struct represents a loop within a track.
 */
struct track_loop
{
    /**
     * \brief Default constructor
     */
    track_loop() :
        is_start_set{false},
        is_end_set{false},
        label{},
        start_sample_offset{-1},
        end_sample_offset{-1},
        colour{}
    {}

    /**
     * \brief Construct a `track loop` from explicit field values
     */
    track_loop(
            bool is_start_set, bool is_end_set, const std::string &label,
            double start_sample_offset, double end_sample_offset,
            const pad_colour &colour) :
        is_start_set{is_start_set},
        is_end_set{is_end_set},
        label{label},
        start_sample_offset{start_sample_offset},
        end_sample_offset{end_sample_offset},
        colour{colour}
    {}

    bool is_start_set;
    bool is_end_set;
    std::string label;
    double start_sample_offset;
    double end_sample_offset;
    pad_colour colour;

    bool is_set() const { return is_start_set && is_end_set; }
};

typedef std::vector<track_hot_cue_point>::const_iterator hot_cue_const_iterator;
typedef std::vector<track_loop>::const_iterator track_loop_const_iterator;

/**
 * The results of track analysis
 */
class performance_data
{
public:
    /**
     * \brief Construct performance data, loading from a database
     */
	performance_data(const database &db, int track_id);

    /**
     * \brief Construct an empty performance data record for a given track,
     *        not yet saved in any database
     */
    performance_data(int track_id);

    /**
     * \brief Destructor
     */
	~performance_data();

    /**
     * \brief Gets the id of the track that this performance data relates to
     */
	int track_id() const;

    /**
     * \brief Gets the sample rate of the track
     */
    double sample_rate() const;

    /**
     * \brief Get the total number of samples in the track
     */
    int_least64_t total_samples() const;

    /**
     * \brief Get the initial musical key of the track
     */
    musical_key key() const;

    /**
     * \brief Get the average loudness of the track
     *
     * The loudness value ranges from zero to one, and is typically close to
     * 0.5 for a well-mastered track.  The exact algorithm for determining
     * loudness is not yet known.
     */
    double average_loudness() const;

    /**
     * \brief Get the default beat grid, i.e. the one detected by automated
     *        analysis
     */
    track_beat_grid default_beat_grid() const;

    /**
     * \brief Get the adjusted beat grid, i.e. the one that may have been
     *        adjusted or tweaked by the user
     *
     * Note that if the beat grid has not been adjusted, then this will be equal
     * to the default beatgrid.
     */
    track_beat_grid adjusted_beat_grid() const;

    /**
     * \brief Gets an iterator pointing to the first hot cue slot
     *
     * Note that there are always 8 hot cues per track in an Engine Prime library
     */
    hot_cue_const_iterator hot_cues_begin() const;

    /**
     * \brief Gets an iterator pointing beyond the last hot cue slot
     *
     * Note that there are always 8 hot cues per track in an Engine Prime library
     */
    hot_cue_const_iterator hot_cues_end() const;

    /**
     * \brief Gets the sample at which the main cue point is set, as determined
     *        by automated analysis of the track
     */
    double default_main_cue_sample_offset() const;

    /**
     * \brief Gets the sample at which the main cue point is set, which may have
     *        been adjusted/tweaked by the user
     *
     * Note that if the user has not adjusted the main cue point, this will be
     * equal to the default main cue point.
     */
    double adjusted_main_cue_sample_offset() const;
    
    /**
     * \brief Gets an iterator pointing to the first loop
     *
     * Note that there are always 8 loops per track in an Engine Prime Library.
     */
    track_loop_const_iterator loops_begin() const;

    /**
     * \brief Gets an iterator pointing beyond the last loop
     *
     * Note that there are always 8 loops per track in an Engine Prime Library.
     */
    track_loop_const_iterator loops_end() const;

    /**
     * \brief Gets the duration of the track, in milliseconds
     *
     * This is calculated from the number of samples in the track, and the
     * sample rate.
     */
    std::chrono::milliseconds duration() const
    {
        auto ms = sample_rate() != 0
            ? 1000 * total_samples() / (int_least64_t)sample_rate()
            : 0;
        return std::chrono::milliseconds{ms};
    }

    /**
     * \brief Get the BPM of the track
     *
     * This is calculated from the adjusted beat grid associated with the track
     * (which is measured in samples), and the sample rate of the track.
     */
	double bpm() const
	{
        if (
                adjusted_beat_grid().last_beat_index -
                adjusted_beat_grid().first_beat_index == 0)
            return 0;

		return sample_rate() * 60 *
			(double)(adjusted_beat_grid().last_beat_index -
			 adjusted_beat_grid().first_beat_index) /
			(adjusted_beat_grid().last_beat_sample_offset -
			 adjusted_beat_grid().first_beat_sample_offset);
	}

    void set_sample_rate(double sample_rate);

    void set_total_samples(int_least64_t total_samples);

    void set_key(musical_key key);

    void set_average_loudness(double average_loudness);

    void set_default_beat_grid(const track_beat_grid &beat_grid);

    void set_default_beat_grid(track_beat_grid &&beat_grid);

    void set_adjusted_beat_grid(const track_beat_grid &beat_grid);

    void set_adjusted_beat_grid(track_beat_grid &&beat_grid);

    /**
     * \brief Set the hot cues
     *
     * Note that the SC5000 Prime allows a maximum of 8 hot cues.  If more than
     * 8 are supplied, only the first 8 will be considered.
     */
    void set_hot_cues(
            hot_cue_const_iterator begin,
            hot_cue_const_iterator end);

    void set_default_main_cue_sample_offset(double sample_offset);

    void set_adjusted_main_cue_sample_offset(double sample_offset);

    /**
     * \brief Set the loops
     *
     * Note that the SC5000 Prime allows a maximum of 8 hot cues.  If more than
     * 8 are supplied, only the first 8 will be considered.
     */
    void set_loops(
            track_loop_const_iterator begin,
            track_loop_const_iterator end);

    /**
     * \brief Save track performance data to a given database
     */
    void save(const database &database);

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

/**
 * \brief Normalise a beat-grid, so that the beat indexes are in the form
 *        normally expected by Engine Prime.
 *
 * By convention, the Engine Prime analyses tracks so that the first beat is
 * at index -4 (yes, negative!) and the last beat is the first beat past the
 * usable end of the track, which may not necessarily be aligned to the first
 * beat of a 4-beat bar.  Therefore, the sample offsets typically recorded by
 * Engine Prime do not usually lie within the actual track.
 */
void normalise_beat_grid(track_beat_grid &beat_grid, double last_sample);

} // enginelibrary
} // djinterop

#endif // DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_HPP