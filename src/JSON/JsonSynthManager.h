#ifndef JsonSynthManager_h
#define JsonSynthManager_h

/// JsonSynthManager ties it all together.

/// A JsonSynthManager is for serialization, deserialization, and management
/// of collections, images, coordinate systems, and projectors.
/// To render JSON output, create all of the necessary instances, initialize
/// their values, hook them up to each other, call compute methods if desired,
/// and call JsonOutput().  The result is a list of strings representing the
/// complete state.
/// Creating a JsonSynthManager and calling JsonInput() will reconstruct all
/// of said objects and calculations.

#include "JsonInput.h"
#include "JsonOutput.h"
#include "FastDelegate.h"
#include <map>

//#define USE_MINI_COMMON

#ifdef USE_MINI_COMMON
    #include "MiniCommon/LivePtr.h"
#else
///////INSERT HEADERS FOR !USE_MINI_COMMON CASE HERE///////
#endif

class JsonCollection;
class JsonSynthOutput;

typedef fastdelegate::FastDelegate1<double> SimpleProgressFxn;

/// This should be a singleton.  When used for output, it holds the various
/// objects while they're being hooked up and computed, ready for
/// serialization with JsonOutput().  When used for input, calling JsonInput()
/// will set up the appropriate objects.  Think of this as the Model part of
/// the Photosynth viewer.
class JsonSynthManager : public RefpObj, public LiveObj {
  public:
    typedef std::map<JsonString, JsonCollection*> Collections;

  private:
    Collections collections_;

    /// Default for progress SimpleProgressFxn FastDelegate-- /dev/null
    void progressDummy(double) {}

    void OutputPreamble(JsonSynthOutput & output,
                        JsonOutDict & root) const;

    JsonCollection * GetOrMakeJsonCollection(const JsonString & name);

  public:
    const Collections & GetCollections() const { 
        return collections_; 
    }

   // static JsonOutDict * CollectionsDict(JsonSynthOutput & output,
//                                         JsonOutDict & root);

    void AddCollection(JsonCollection & collection);

    /// The we-manage-it version.  Refcounted in either case.
    JsonCollection * AddCollection(const JsonString & name);
    JsonCollection * AddCollection(const char * name);

    /// Computes panorama and halo clusters;
    /// Uses directed maximum spanning tree algorithms to calculate the
    /// discrete neighbors-- left, right, up, down, in, out;
    /// Computes quads and peers;
    /// Computes tour using traveling salesman.
    /// Do this after setting up one or more JsonCollections, JsonCoordSystems,
    /// JsonProjectors and JsonImages and assigning any desired properties,
    /// including manual neighbors, quads and peers.
    /// May take many seconds, especially for large synths.
    /// Overwrites existing discrete neighbors with computed values; but if
    /// a neighbor was assigned manually and isn't reassigned algorithmically,
    /// doesn't overwrite the existing value.
    /// Augments any manually specified peers and quads with computed values.
    /// Be sure to initialize all projector matches first.
    /// We don't need to do this if we've deserialized and it was done prior
    /// to serializing (at synth time).
    void ComputeNavigationRelationships(SimpleProgressFxn progress);
    void ComputeNavigationRelationships();

    /// JsonInput() returns the number of characters in @input actually
    /// parsed.  If the input was a complete stream, that should be
    /// input.size().  But this input function is quite robust.  It handles
    /// the case where the input was originally broken into chunks using
    /// JsonSynthOutputStreams, but the input fed in here has been
    /// concatenated together.  (Multiple complete concatenated streams should
    /// also get parsed at once, unless the default @maxStreams parameter
    /// is set nonnegative, in which case JsonInput will only parse up to this
    /// many streams.)  It also handles cases where @input is partial, meaning
    /// ends in mid-stream.  In this case, a subsequent call should be made
    /// to JsonInput with the remainder of the previous string concatenated
    /// with further input.
    /// Finally, JsonInput sets up the synth data structures robustly,
    /// handling the case where streams arrive out of order.
    int JsonInput(const JsonString & input, int maxStreams=-1);
    int JsonInput(const char * input, int maxStreams=-1) {
        return JsonInput(JsonString(input), maxStreams);
    }

};

#endif // JsonSynthManager_h
