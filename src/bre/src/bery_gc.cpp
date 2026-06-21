#include "../include/bery_gc.h"
#include "../include/bery_runtime.h"
#include "../include/bery_object.h"
#include "../include/bery_gc_roots.h"
#include "../include/bery_alloc.h"
#include "../include/bery_type_registry.h"
#include <cstdlib>
#include <vector>


bool bery_gc_should_collect() {
    return g_beryRuntime.allocationCount >= BERY_GC_ALLOC_THRESHHOLD || g_beryRuntime.totalAllocated >=BERY_GC_HEAP_SIZE_THRESHHOLD;
}

static void markObject(BeryObjectHeader* header, std::vector<BeryObjectHeader*>& list) {
    if (header->marked) return;
    header->marked = 1;
    list.push_back(header);
}

static void markPhase() {
    std::vector<BeryObjectHeader*> list;
    Root* root = bery_gc_roots_head();

    while(root) {
        void* obj = *(root->slot);
        if(obj) {
            BeryObjectHeader* header = bery_header_from_payload(obj);
            markObject(header, list);
        }

        root = root->next;
    }

    while(!list.empty()) {
        BeryObjectHeader* header = list.back();
        list.pop_back();

        BeryTypeInfo* type = bery_type_lookup(header->typeId);
        if(!type) continue;

        char* payload = reinterpret_cast<char*>(header) + sizeof(BeryObjectHeader);
        for (size_t i = 0; i < type->pointerFieldCount; i++) {
            size_t offset = type->pointerFields[i].offset;
            void* fieldValye = *reinterpret_cast<void**>(payload + offset);
            if(fieldValye) {
                BeryObjectHeader* fieldHeader = bery_header_from_payload(fieldValye);
                markObject(fieldHeader, list);
            }
        }
    }
}

static void sweepPhase() {
    BeryObjectHeader** current = &g_beryRuntime.heapHead;
    while (*current) {
        BeryObjectHeader* obj = *current;
        if (obj->marked) {
            obj->marked = 0;
            current = &obj->next;
        } else {
            *current = obj->next;
            g_beryRuntime.totalAllocated -= obj->size;
            g_beryRuntime.totalObjectsLive -= 1;
            free(obj);
        }
    }
}

void bery_gc_collect() {
    markPhase();
    sweepPhase();
    g_beryRuntime.allocationCount = 0;
}