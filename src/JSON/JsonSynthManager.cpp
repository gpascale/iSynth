//#include "Stable.h"
#include "JsonSynthManager.h"
#include "GregJsonCollection.h"
//#include "JsonSynthOutput.h"

/*
JsonOutDict * JsonSynthManager::CollectionsDict(JsonSynthOutput & output,
                                                JsonOutDict & root) {
    JsonOutDict * collectionsDict = new JsonOutDict();
    root.add(output.Key("collections"), collectionsDict);
    return collectionsDict;
}
*/

void JsonSynthManager::AddCollection(JsonCollection & collection) {
    const JsonString & name = collection.getName();
    Collections::iterator it = collections_.find(name);
    if (it != collections_.end()) {
        JsonThrow("duplicate collection name");
    }
    collections_.insert(Collections::value_type(name, &collection));
}

JsonCollection * JsonSynthManager::AddCollection(const JsonString & name) {
    JsonCollection *C = new JsonCollection(name);
    AddCollection(*C);
    return C;
}
JsonCollection * JsonSynthManager::AddCollection(const char * name) {
    return AddCollection(JsonString(name));
}

void JsonSynthManager::ComputeNavigationRelationships(SimpleProgressFxn progress) {
    /*
    Collections::iterator it, it1 = collections_.end();
    for (it=collections_.begin(); it!=it1; ++it) {
        it->second->ComputeNavigationRelationships();
    }
    */
}

JsonCollection * JsonSynthManager::GetOrMakeJsonCollection(const JsonString & name) {
    JsonCollection * result = 0;
    Collections::const_iterator it = collections_.find(name);
    if (it == collections_.end()) {
        result = AddCollection(name);
    } else {
        result = it->second;
    }
    b_assert(result);
    return result;
}

int JsonSynthManager::JsonInput(const JsonString & input, int maxStreams) {
    size_t nCharsParsed = 0;
    try {
        JsonParser J;
        JsonSynthAliases aliases;
        nCharsParsed = J.parse(input, aliases, false); // don't specialize

        // asserts that this is in fact a JSON synth, and that the version is >=1.0
        if (double(J("_json_synth")) < 1.01) {
            JsonThrow("pre-1.01 version of json-synth");
        }

        const JsonInDict & jCollections = J("l").ToDict();
        JsonArrayStringV collectionKeys;
        jCollections.keys(collectionKeys);

        const size_t nColl = collectionKeys.size();
        for (size_t iColl=0; iColl<nColl; ++iColl) {
            const JsonString & collKey = collectionKeys[iColl];
            JsonCollection * coll = GetOrMakeJsonCollection(collKey);
            JsonInDict jCollection = jCollections(collKey).ToDict();
			
			JSON_PRINTF("parsing collection %d\n", iColl);
			
            coll->Parse(jCollection);
        }

    } catch (JsonException & ) {
        // We could surface more details here about what went wrong.
        // As it is, the caller should note that zero characters parsed means
        // something's wrong-- don't just keep calling back.
    }
    return int(nCharsParsed);
}
