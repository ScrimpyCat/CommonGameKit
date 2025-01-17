const size_t ArchetypeComponentIndexes[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 7,
    0, 1, 2, 3, 4, 6, 7,
    0, 1, 2, 3, 5, 6, 7,
    0, 1, 2, 4, 5, 6, 7,
    0, 1, 3, 4, 5, 6, 7,
    0, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 7,
    0, 1, 2, 3, 5, 7,
    0, 1, 2, 3, 6, 7,
    0, 1, 2, 4, 5, 7,
    0, 1, 2, 4, 6, 7,
    0, 1, 2, 5, 6, 7,
    0, 1, 3, 4, 6, 7,
    0, 1, 3, 5, 6, 7,
    0, 1, 4, 5, 6, 7,
    0, 2, 3, 5, 6, 7,
    0, 2, 4, 5, 6, 7,
    0, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 7,
    0, 1, 2, 4, 7,
    0, 1, 2, 5, 7,
    0, 1, 2, 6, 7,
    0, 1, 3, 4, 7,
    0, 1, 3, 5, 7,
    0, 1, 3, 6, 7,
    0, 1, 4, 5, 7,
    0, 1, 4, 6, 7,
    0, 1, 5, 6, 7,
    0, 2, 3, 6, 7,
    0, 2, 4, 5, 7,
    0, 2, 4, 6, 7,
    0, 2, 5, 6, 7,
    0, 3, 4, 6, 7,
    0, 3, 5, 6, 7,
    0, 4, 5, 6, 7,
    0, 1, 2, 7,
    0, 1, 3, 7,
    0, 1, 4, 7,
    0, 1, 5, 7,
    0, 1, 6, 7,
    0, 2, 3, 7,
    0, 2, 4, 7,
    0, 2, 5, 7,
    0, 2, 6, 7,
    0, 3, 4, 7,
    0, 3, 5, 7,
    0, 3, 6, 7,
    0, 4, 5, 7,
    0, 4, 6, 7,
    0, 5, 6, 7,
    0, 1, 7,
    0, 2, 7,
    0, 3, 7,
    0, 4, 7,
    0, 5, 7,
    0, 6, 7,
    0, 7,
};

#define INDEX_0_4 202, 2
#define INDEX_1_3_5 93, 3
#define INDEX_0_5 263, 2
#define INDEX_2_4_5_6 31, 4
#define INDEX_2_4_7 129, 3
#define INDEX_1_3_6_7 153, 4
#define INDEX_0_1_6 223, 3
#define INDEX_1_2_5_6_7 81, 5
#define INDEX_0_1_2_5_6 80, 5
#define INDEX_1_3_4 37, 3
#define INDEX_0_2_3_4_5_6 43, 6
#define INDEX_0_1_2_3_4_5_6_7 0, 8
#define INDEX_0_1_2_4_5 29, 5
#define INDEX_0_1_2_4 29, 4
#define INDEX_2_3_4_5 2, 4
#define INDEX_2_3_6_7 64, 4
#define INDEX_1_2_4_5_6_7 30, 6
#define INDEX_2_4_5_6_7 31, 5
#define INDEX_1_2_3_5_6 23, 5
#define INDEX_0_3_5_7 247, 4
#define INDEX_3_4_7 53, 3
#define INDEX_0_1_2_4_5_6_7 29, 7
#define INDEX_1_6 224, 2
#define INDEX_0_1_2_6_7 137, 5
#define INDEX_0_1_4_7 215, 4
#define INDEX_2_5_7 134, 3
#define INDEX_0_1_2_3_4_5_7 8, 7
#define INDEX_0_1_2_5_7 132, 5
#define INDEX_0_2 43, 2
#define INDEX_0_1_2_4_5_6 29, 6
#define INDEX_0_2_4_5 110, 4
#define INDEX_4 4, 1
#define INDEX_0_6_7 282, 3
#define INDEX_1_3_4_5_6_7 37, 6
#define INDEX_0_3_6 251, 3
#define INDEX_1_2_4_5 30, 4
#define INDEX_0_2_5_6 187, 4
#define INDEX_1_2 1, 2
#define INDEX_0_1_2_3_4_5_6 0, 7
#define INDEX_5_6_7 5, 3
#define INDEX_2_4_6_7 76, 4
#define INDEX_0_1_2_3_4_6 15, 6
#define INDEX_0_3_7 273, 3
#define INDEX_0_2_3_7 227, 4
#define INDEX_0_1_2_3_4 0, 5
#define INDEX_0_1_2 0, 3
#define INDEX_3_6 65, 2
#define INDEX_0_1_2_6 137, 4
#define INDEX_0_3_4 116, 3
#define INDEX_4_5_7 12, 3
#define INDEX_0_6 282, 2
#define INDEX_0_2_5_6_7 187, 5
#define INDEX_0_2_4 110, 3
#define INDEX_0_2_3_4_5 43, 5
#define INDEX_0_5_7 279, 3
#define INDEX_2_3_5 24, 3
#define INDEX_2_3_5_7 58, 4
#define INDEX_0_3 116, 2
#define INDEX_0_4_5 202, 3
#define INDEX_7 7, 1
#define INDEX_0_1_2_4_5_7 68, 6
#define INDEX_4_5_6 4, 3
#define INDEX_1_4_5_7 158, 4
#define INDEX_0_4_5_7 255, 4
#define INDEX_6_7 6, 2
#define INDEX_2_3_5_6_7 24, 5
#define INDEX_1_2_3_7 123, 4
#define INDEX_0_3_4_6_7 192, 5
#define INDEX_1_2_3_4_5_6 1, 6
#define INDEX_0_3_5_6_7 197, 5
#define INDEX_0_1_3_4_5_6 36, 6
#define INDEX_2_3_4_5_6 2, 5
#define INDEX_1_2_6_7 138, 4
#define INDEX_0_1_2_3_6_7 62, 6
#define INDEX_0_1 0, 2
#define INDEX_2_4_5 31, 3
#define INDEX_2_3_4 2, 3
#define INDEX_4_7 54, 2
#define INDEX_2_3_5_6 24, 4
#define INDEX_0_2_4_6_7 182, 5
#define INDEX_0_1_2_3_6 62, 5
#define INDEX_5_6 5, 2
#define INDEX_0_1_7 267, 3
#define INDEX_6 6, 1
#define INDEX_0_1_3_4_5_6_7 36, 7
#define INDEX_0_1_3_7 211, 4
#define INDEX_0_1_3_4_7 142, 5
#define INDEX_0_2_3_4 43, 4
#define INDEX_0_3_4_5_6_7 116, 6
#define INDEX_1_4_5 99, 3
#define INDEX_1_2_5_6 81, 4
#define INDEX_0_1_3_4_6_7 86, 6
#define INDEX_0_1_2_4_7 127, 5
#define INDEX_1_3_7 212, 3
#define INDEX_1_2_3_6_7 63, 5
#define INDEX_0_3_4_5_6 116, 5
#define INDEX_3_4_5_6 3, 4
#define INDEX_1_2_4_6_7 75, 5
#define INDEX_1_2_3_6 63, 4
#define INDEX_3_4 3, 2
#define INDEX_1_6_7 224, 3
#define INDEX_0_1_2_3_4_5 0, 6
#define INDEX_0_1_3 36, 3
#define INDEX_2_3 2, 2
#define INDEX_1_7 268, 2
#define INDEX_0_2_7 270, 3
#define INDEX_0_2_4_5_7 177, 5
#define INDEX_0_1_2_4_6_7 74, 6
#define INDEX_0_5_6 263, 3
#define INDEX_0_1_3_5 92, 4
#define INDEX_0_1_2_3 0, 4
#define INDEX_3_4_5_6_7 3, 5
#define INDEX_0_2_3_6 172, 4
#define INDEX_0_2_3_6_7 172, 5
#define INDEX_0_1_2_5 80, 4
#define INDEX_0_2_4_7 231, 4
#define INDEX_0_1_2_7 207, 4
#define INDEX_2_4 31, 2
#define INDEX_1_2_3_4_5 1, 5
#define INDEX_2_4_6 76, 3
#define INDEX_0_7 285, 2
#define INDEX_3_4_6_7 18, 4
#define INDEX_2_3_4_5_7 10, 5
#define INDEX_2_3_4_5_6_7 2, 6
#define INDEX_5_7 13, 2
#define INDEX_0_2_5_7 235, 4
#define INDEX_1_2_3_4_7 51, 5
#define INDEX_1_2_4_7 128, 4
#define INDEX_1_4_5_6_7 99, 5
#define INDEX_0_2_4_5_6 110, 5
#define INDEX_2_5 82, 2
#define INDEX_4_6_7 19, 3
#define INDEX_0_1_3_6_7 152, 5
#define INDEX_0_1_5 167, 3
#define INDEX_1_2_4_5_7 69, 5
#define INDEX_0 0, 1
#define INDEX_0_3_5_6 197, 4
#define INDEX_1_2_3_4_5_6_7 1, 7
#define INDEX_0_4_6 259, 3
#define INDEX_1_3_4_5_6 37, 5
#define INDEX_0_1_2_3_5_7 56, 6
#define INDEX_1_2_4 30, 3
#define INDEX_0_3_4_5 116, 4
#define INDEX_3_5_6 25, 3
#define INDEX_3_7 125, 2
#define INDEX_0_1_3_5_7 147, 5
#define INDEX_1_2_4_5_6 30, 5
#define INDEX_2_6_7 139, 3
#define INDEX_0_1_2_3_4_7 50, 6
#define INDEX_3_4_6 18, 3
#define INDEX_2_5_6 82, 3
#define INDEX_1_2_3_4_6 16, 5
#define INDEX_0_3_5 197, 3
#define INDEX_1_2_3_4_5_7 9, 6
#define INDEX_0_1_4_5_6 98, 5
#define INDEX_0_2_4_5_6_7 110, 6
#define INDEX_3_4_5 3, 3
#define INDEX_0_1_4 98, 3
#define INDEX_1_2_7 208, 3
#define INDEX_0_2_6 239, 3
#define INDEX_1_4_7 216, 3
#define INDEX_4_6 19, 2
#define INDEX_0_2_4_6 182, 4
#define INDEX_1_2_3_5 23, 4
#define INDEX_4_5_6_7 4, 4
#define INDEX_0_1_3_5_6 92, 5
#define INDEX_0_1_2_3_4_6_7 15, 7
#define INDEX_0_1_2_3_5_6 22, 6
#define INDEX_0_3_4_6 192, 4
#define INDEX_0_1_5_7 219, 4
#define INDEX_0_1_2_5_6_7 80, 6
#define INDEX_1_2_3 1, 3
#define INDEX_3_5 25, 2
#define INDEX_2_3_4_7 52, 4
#define INDEX_5 5, 1
#define INDEX_1_3_4_6_7 87, 5
#define INDEX_0_1_2_3_5_6_7 22, 7
#define INDEX_2_4_5_7 70, 4
#define INDEX_1_3 37, 2
#define INDEX_1_2_6 138, 3
#define INDEX_1_2_3_4 1, 4
#define INDEX_0_1_3_4 36, 4
#define INDEX_1_3_5_6_7 93, 5
#define INDEX_2_5_6_7 82, 4
#define INDEX_0_4_5_6 202, 4
#define INDEX_0_1_2_3_7 122, 5
#define INDEX_1_2_5_7 133, 4
#define INDEX_2_3_7 124, 3
#define INDEX_0_1_5_6_7 167, 5
#define INDEX_3_5_7 59, 3
#define INDEX_0_1_4_5_7 157, 5
#define INDEX_2 2, 1
#define INDEX_1_5_7 220, 3
#define INDEX_0_3_4_7 243, 4
#define INDEX_2_7 209, 2
#define INDEX_0_1_4_5 98, 4
#define INDEX_0_2_3 43, 3
#define INDEX_0_2_3_4_5_6_7 43, 7
#define INDEX_3_5_6_7 25, 4
#define INDEX_0_2_3_5_6_7 104, 6
#define INDEX_0_1_3_4_5 36, 5
#define INDEX_0_1_3_4_6 86, 5
#define INDEX_3_6_7 65, 3
#define INDEX_0_2_3_5 104, 4
#define INDEX_0_1_4_5_6_7 98, 6
#define INDEX_0_2_5 187, 3
#define INDEX_1_3_4_5 37, 4
#define INDEX_1_3_5_6 93, 4
#define INDEX_0_1_4_6 162, 4
#define INDEX_2_3_6 64, 3
#define INDEX_0_1_6_7 223, 4
#define INDEX_0_1_2_4_6 74, 5
#define INDEX_3_4_5_7 11, 4
#define INDEX_1_2_3_5_6_7 23, 6
#define INDEX_1_4_6_7 163, 4
#define INDEX_0_1_4_6_7 162, 5
#define INDEX_1_3_6 153, 3
#define INDEX_1_2_4_6 75, 4
#define INDEX_1_2_3_5_7 57, 5
#define INDEX_0_3_6_7 251, 4
#define INDEX_0_4_5_6_7 202, 5
#define INDEX_1_3_4_6 87, 4
#define INDEX_2_6 139, 2
#define INDEX_0_1_5_6 167, 4
#define INDEX_2_3_4_6_7 17, 5
#define INDEX_1_2_3_4_6_7 16, 6
#define INDEX_0_5_6_7 263, 4
#define INDEX_1_4_5_6 99, 4
#define INDEX_1 1, 1
#define INDEX_1_3_5_7 148, 4
#define INDEX_0_1_3_5_6_7 92, 6
#define INDEX_0_1_2_3_5 22, 5
#define INDEX_1_5_6 168, 3
#define INDEX_0_4_7 276, 3
#define INDEX_1_5_6_7 168, 4
#define INDEX_1_2_5 81, 3
#define INDEX_1_5 168, 2
#define INDEX_1_4 99, 2
#define INDEX_3 3, 1
#define INDEX_1_3_4_7 143, 4
#define INDEX_4_5 4, 2
#define INDEX_0_4_6_7 259, 4
#define INDEX_1_4_6 163, 3
#define INDEX_2_3_4_6 17, 4
#define INDEX_0_1_3_6 152, 4
#define INDEX_0_2_3_5_6 104, 5
#define INDEX_0_2_6_7 239, 4

_Static_assert(64 == ECS_LOCAL_COMPONENT_MAX, "Regenerate file with new ECS_LOCAL_COMPONENT_MAX value.");

#define COMP_A (ECSComponentStorageTypeArchetype | 0)
#define COMP_B (ECSComponentStorageTypeArchetype | 1)
#define COMP_C (ECSComponentStorageTypeArchetype | 2)
#define COMP_D (ECSComponentStorageTypeArchetype | 3)
#define COMP_E (ECSComponentStorageTypeArchetype | 4)
#define ARCH_K (ECSComponentStorageTypeArchetype | ECSComponentStorageModifierDestructor | 5)
#define ARCH_H (ECSComponentStorageTypeArchetype | ECSComponentStorageModifierDestructor | 6)
#define ARCH_TAG (ECSComponentStorageTypeArchetype | ECSComponentStorageModifierTag | 7)
#define COMP_F (ECSComponentStorageTypePacked | 0)
#define ECS_MONITOR_COMPONENT (ECSComponentStorageTypePacked | ECSComponentStorageModifierDuplicate | ECSComponentStorageModifierDestructor | (ECS_MONITOR_COMPONENT_ID))
#define COMP_J (ECSComponentStorageTypePacked | 2)
#define PACKED_K (ECSComponentStorageTypePacked | ECSComponentStorageModifierDestructor | 3)
#define PACKED_H (ECSComponentStorageTypePacked | ECSComponentStorageModifierDestructor | 4)
#define CHECK_RUN_STATE_TAG (ECSComponentStorageTypePacked | ECSComponentStorageModifierTag | 5)
#define DESTROY_ME_TAG (ECSComponentStorageTypePacked | ECSComponentStorageModifierTag | ECSComponentStorageModifierDestructor | 6)
#define DUPLICATE_A (ECSComponentStorageTypePacked | ECSComponentStorageModifierDuplicate | ECSComponentStorageModifierDestructor | 7)
#define PACKED_TAG (ECSComponentStorageTypePacked | ECSComponentStorageModifierTag | 8)
#define COMP_G (ECSComponentStorageTypePacked | 10)
#define COMP_H (ECSComponentStorageTypeIndexed | 0)
#define COMP_I (ECSComponentStorageTypeIndexed | 1)
#define ECS_NAME_COMPONENT (ECSComponentStorageTypeIndexed | ECSComponentStorageModifierDestructor | (ECS_NAME_COMPONENT_ID))
#define INDEXED_K (ECSComponentStorageTypeIndexed | ECSComponentStorageModifierDestructor | 3)
#define INDEXED_H (ECSComponentStorageTypeIndexed | ECSComponentStorageModifierDestructor | 4)
#define INDEXED_TAG (ECSComponentStorageTypeIndexed | ECSComponentStorageModifierTag | 5)
#define LOCAL_K (ECSComponentStorageTypeLocal | (0 << 7) | ECSComponentStorageModifierDestructor | 0)
#define LOCAL_H (ECSComponentStorageTypeLocal | ((((LOCAL_K & ~ECSComponentStorageMask) >> 7) + sizeof(LocalK)) << 7) | ECSComponentStorageModifierDestructor | 1)
#define LOCAL_DUPLICATE_B (ECSComponentStorageTypeLocal | ((((LOCAL_H & ~ECSComponentStorageMask) >> 7) + sizeof(LocalH)) << 7) | ECSComponentStorageModifierDuplicate | 2)
#define LOCAL_TAG (ECSComponentStorageTypeLocal | ((((LOCAL_DUPLICATE_B & ~ECSComponentStorageMask) >> 7) + sizeof(CCArray)) << 7) | ECSComponentStorageModifierTag | 3)

#define LOCAL_STORAGE_SIZE (((LOCAL_TAG & ~ECSComponentStorageMask) >> 7) + sizeof(LocalTag))

const ECSComponentID ComponentIDs[ECS_COMPONENT_MAX] = {
    [ECS_COMPONENT_BASE_INDEX(COMP_A)] = COMP_A,
    [ECS_COMPONENT_BASE_INDEX(COMP_B)] = COMP_B,
    [ECS_COMPONENT_BASE_INDEX(COMP_C)] = COMP_C,
    [ECS_COMPONENT_BASE_INDEX(COMP_D)] = COMP_D,
    [ECS_COMPONENT_BASE_INDEX(COMP_E)] = COMP_E,
    [ECS_COMPONENT_BASE_INDEX(ARCH_K)] = ARCH_K,
    [ECS_COMPONENT_BASE_INDEX(ARCH_H)] = ARCH_H,
    [ECS_COMPONENT_BASE_INDEX(ARCH_TAG)] = ARCH_TAG,
    [ECS_COMPONENT_BASE_INDEX(COMP_F)] = COMP_F,
    [ECS_COMPONENT_BASE_INDEX(ECS_MONITOR_COMPONENT)] = ECS_MONITOR_COMPONENT,
    [ECS_COMPONENT_BASE_INDEX(COMP_J)] = COMP_J,
    [ECS_COMPONENT_BASE_INDEX(PACKED_K)] = PACKED_K,
    [ECS_COMPONENT_BASE_INDEX(PACKED_H)] = PACKED_H,
    [ECS_COMPONENT_BASE_INDEX(CHECK_RUN_STATE_TAG)] = CHECK_RUN_STATE_TAG,
    [ECS_COMPONENT_BASE_INDEX(DESTROY_ME_TAG)] = DESTROY_ME_TAG,
    [ECS_COMPONENT_BASE_INDEX(DUPLICATE_A)] = DUPLICATE_A,
    [ECS_COMPONENT_BASE_INDEX(PACKED_TAG)] = PACKED_TAG,
    [ECS_COMPONENT_BASE_INDEX(COMP_G)] = COMP_G,
    [ECS_COMPONENT_BASE_INDEX(COMP_H)] = COMP_H,
    [ECS_COMPONENT_BASE_INDEX(COMP_I)] = COMP_I,
    [ECS_COMPONENT_BASE_INDEX(ECS_NAME_COMPONENT)] = ECS_NAME_COMPONENT,
    [ECS_COMPONENT_BASE_INDEX(INDEXED_K)] = INDEXED_K,
    [ECS_COMPONENT_BASE_INDEX(INDEXED_H)] = INDEXED_H,
    [ECS_COMPONENT_BASE_INDEX(INDEXED_TAG)] = INDEXED_TAG,
    [ECS_COMPONENT_BASE_INDEX(LOCAL_K)] = LOCAL_K,
    [ECS_COMPONENT_BASE_INDEX(LOCAL_H)] = LOCAL_H,
    [ECS_COMPONENT_BASE_INDEX(LOCAL_DUPLICATE_B)] = LOCAL_DUPLICATE_B,
    [ECS_COMPONENT_BASE_INDEX(LOCAL_TAG)] = LOCAL_TAG,
};

const size_t ArchetypeComponentSizes[ECS_ARCHETYPE_COMPONENT_MAX] = {
    sizeof(CompA),
    sizeof(CompB),
    sizeof(CompC),
    sizeof(CompD),
    sizeof(CompE),
    sizeof(ArchK),
    sizeof(ArchH),
    sizeof(ArchTag),
};
const size_t DuplicateArchetypeComponentSizes[ECS_ARCHETYPE_COMPONENT_MAX] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
const size_t PackedComponentSizes[ECS_PACKED_COMPONENT_MAX] = {
    sizeof(CompF),
    sizeof(CCArray),
    sizeof(CompJ),
    sizeof(PackedK),
    sizeof(PackedH),
    sizeof(CheckRunStateTag),
    sizeof(DestroyMeTag),
    sizeof(CCArray),
    sizeof(PackedTag),
    0,
    sizeof(CompG),
};
const size_t DuplicatePackedComponentSizes[ECS_PACKED_COMPONENT_MAX] = {
    0,
    sizeof(ECSMonitorComponent),
    0,
    0,
    0,
    0,
    0,
    sizeof(DuplicateA),
    0,
    0,
    0,
};
const size_t IndexedComponentSizes[ECS_INDEXED_COMPONENT_MAX] = {
    sizeof(CompH),
    sizeof(CompI),
    sizeof(ECSNameComponent),
    sizeof(IndexedK),
    sizeof(IndexedH),
    sizeof(IndexedTag),
};
const size_t DuplicateIndexedComponentSizes[ECS_INDEXED_COMPONENT_MAX] = {
    0,
    0,
    0,
    0,
    0,
    0,
};
const size_t LocalComponentSizes[ECS_LOCAL_COMPONENT_MAX] = {
    sizeof(LocalK),
    sizeof(LocalH),
    sizeof(CCArray),
    sizeof(LocalTag),
};
const size_t DuplicateLocalComponentSizes[ECS_LOCAL_COMPONENT_MAX] = {
    0,
    0,
    sizeof(LocalDuplicateB),
    0,
};

const ECSComponentDestructor ArchetypeComponentDestructors[ECS_ARCHETYPE_COMPONENT_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TrackDestructor,
    TestDestructor,
    NULL,
};
const ECSComponentDestructor DuplicateArchetypeComponentDestructors[ECS_ARCHETYPE_COMPONENT_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
const ECSComponentDestructor PackedComponentDestructors[ECS_PACKED_COMPONENT_MAX] = {
    NULL,
    ECSDuplicateDestructor,
    NULL,
    TrackDestructor,
    TestDestructor,
    NULL,
    MutationDestructor,
    ECSDuplicateDestructor,
    NULL,
    NULL,
    NULL,
};
const ECSComponentDestructor DuplicatePackedComponentDestructors[ECS_PACKED_COMPONENT_MAX] = {
    NULL,
    ECSMonitorComponentDestructor,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TestDestructor,
    NULL,
    NULL,
    NULL,
};
const ECSComponentDestructor IndexedComponentDestructors[ECS_INDEXED_COMPONENT_MAX] = {
    NULL,
    NULL,
    ECSNameComponentDestructor,
    TrackDestructor,
    TestDestructor,
    NULL,
};
const ECSComponentDestructor DuplicateIndexedComponentDestructors[ECS_INDEXED_COMPONENT_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
const ECSComponentDestructor LocalComponentDestructors[ECS_LOCAL_COMPONENT_MAX] = {
    TrackDestructor,
    TestDestructor,
    ECSDuplicateDestructor,
    NULL,
};
const ECSComponentDestructor DuplicateLocalComponentDestructors[ECS_LOCAL_COMPONENT_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
};

const CCReflectType ArchetypeComponentTypes[ECS_ARCHETYPE_COMPONENT_MAX] = {
    &CC_REFLECT(CompA),
    &CC_REFLECT(CompB),
    &CC_REFLECT(CompC),
    &CC_REFLECT(CompD),
    &CC_REFLECT(CompE),
    &CC_REFLECT(ArchK),
    &CC_REFLECT(ArchH),
    &CC_REFLECT(ArchTag),
};
const CCReflectType DuplicateArchetypeComponentTypes[ECS_ARCHETYPE_COMPONENT_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
const CCReflectType PackedComponentTypes[ECS_PACKED_COMPONENT_MAX] = {
    &CC_REFLECT(CompF),
    NULL, //&CC_REFLECT_CCArray(&CC_REFLECT(ECSMonitorComponent), CC_STD_ALLOCATOR, ECS_DUPLICATE_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(ECS_COMPONENT_INDEX(ECS_MONITOR_COMPONENT))),
    &CC_REFLECT(CompJ),
    &CC_REFLECT(PackedK),
    &CC_REFLECT(PackedH),
    &CC_REFLECT(CheckRunStateTag),
    &CC_REFLECT(DestroyMeTag),
    &CC_REFLECT_CCArray(&CC_REFLECT(DuplicateA), CC_STD_ALLOCATOR, ECS_DUPLICATE_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(ECS_COMPONENT_INDEX(DUPLICATE_A))),
    &CC_REFLECT(PackedTag),
    NULL,
    &CC_REFLECT(CompG),
};
const CCReflectType DuplicatePackedComponentTypes[ECS_PACKED_COMPONENT_MAX] = {
    NULL,
    NULL, //&CC_REFLECT(ECSMonitorComponent),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &CC_REFLECT(DuplicateA),
    NULL,
    NULL,
    NULL,
};
const CCReflectType IndexedComponentTypes[ECS_INDEXED_COMPONENT_MAX] = {
    &CC_REFLECT(CompH),
    &CC_REFLECT(CompI),
    &CC_REFLECT(ECSNameComponent),
    &CC_REFLECT(IndexedK),
    &CC_REFLECT(IndexedH),
    &CC_REFLECT(IndexedTag),
};
const CCReflectType DuplicateIndexedComponentTypes[ECS_INDEXED_COMPONENT_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
const CCReflectType LocalComponentTypes[ECS_LOCAL_COMPONENT_MAX] = {
    &CC_REFLECT(LocalK),
    &CC_REFLECT(LocalH),
    &CC_REFLECT_CCArray(&CC_REFLECT(LocalDuplicateB), CC_STD_ALLOCATOR, ECS_DUPLICATE_LOCAL_COMPONENT_ARRAY_CHUNK_SIZE(ECS_COMPONENT_INDEX(LOCAL_DUPLICATE_B))),
    &CC_REFLECT(LocalTag),
};
const CCReflectType DuplicateLocalComponentTypes[ECS_LOCAL_COMPONENT_MAX] = {
    NULL,
    NULL,
    &CC_REFLECT(LocalDuplicateB),
    NULL,
};

const ECSArchetypePointer ArchetypeDependencies[] = {
#define ARCHETYPE_DEPENDENCIES_CompA_CompC_CompD 0
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_D)]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, COMP_E)]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2_3 },
#define ARCHETYPE_DEPENDENCIES_CompA_CompB_CompC 32
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_C)]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_E)]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1_2 },
#define ARCHETYPE_DEPENDENCIES_CompC_CompD 64
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_C, COMP_D)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_D)]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, COMP_D)]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, COMP_E)]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1_2 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2_3 },
#define ARCHETYPE_DEPENDENCIES_ArchTag_CompA 128
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_3 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_4 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_5 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_6 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_6 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_6 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_6 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_6 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_6 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_7 },
#define ARCHETYPE_DEPENDENCIES_CompA_CompC 192
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_C)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_C)]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_D)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_E)]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_2 },
#define ARCHETYPE_DEPENDENCIES_CompA_CompB 256
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_B)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_C)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_D)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0_1 },
#define ARCHETYPE_DEPENDENCIES_CompC 320
    { offsetof(ECSContext, archetypes1[ECS_ARCHETYPE1_INDEX(COMP_C)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_C)]), INDEX_1 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, COMP_C)]), INDEX_1 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_C, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_C, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_C)]), INDEX_2 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_D)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, COMP_D)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_E)]), INDEX_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_2 },
#define ARCHETYPE_DEPENDENCIES_CompB 448
    { offsetof(ECSContext, archetypes1[ECS_ARCHETYPE1_INDEX(COMP_B)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_B)]), INDEX_1 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, COMP_C)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_B, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_C)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_D)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_B, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_1 },
#define ARCHETYPE_DEPENDENCIES_CompA 576
    { offsetof(ECSContext, archetypes1[ECS_ARCHETYPE1_INDEX(COMP_A)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_B)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_C)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes2[ECS_ARCHETYPE2_INDEX(COMP_A, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_C)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_B, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes3[ECS_ARCHETYPE3_INDEX(COMP_A, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_D)]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_B, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes4[ECS_ARCHETYPE4_INDEX(COMP_A, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E)]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_B, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes5[ECS_ARCHETYPE5_INDEX(COMP_A, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_C, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_B, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes6[ECS_ARCHETYPE6_INDEX(COMP_A, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_C, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_B, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes7[ECS_ARCHETYPE7_INDEX(COMP_A, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
    { offsetof(ECSContext, archetypes8[ECS_ARCHETYPE8_INDEX(COMP_A, COMP_B, COMP_C, COMP_D, COMP_E, (ARCH_K & ~ECSComponentStorageMask), (ARCH_H & ~ECSComponentStorageMask), (ARCH_TAG & ~ECSComponentStorageMask))]), INDEX_0 },
};

#define ARCHETYPE1_DEPENDENCIES_COUNT 128
#define ARCHETYPE2_DEPENDENCIES_COUNT 64
#define ARCHETYPE3_DEPENDENCIES_COUNT 32
#define ARCHETYPE4_DEPENDENCIES_COUNT 16
#define ARCHETYPE5_DEPENDENCIES_COUNT 8
#define ARCHETYPE6_DEPENDENCIES_COUNT 4
#define ARCHETYPE7_DEPENDENCIES_COUNT 2
#define ARCHETYPE8_DEPENDENCIES_COUNT 1

#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE1(x1) { .count = ARCHETYPE1_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE2(x1, x2) { .count = ARCHETYPE2_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE3(x1, x2, x3) { .count = ARCHETYPE3_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2##_##x3] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE4(x1, x2, x3, x4) { .count = ARCHETYPE4_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2##_##x3##_##x4] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE5(x1, x2, x3, x4, x5) { .count = ARCHETYPE5_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2##_##x3##_##x4##_##x5] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE6(x1, x2, x3, x4, x5, x6) { .count = ARCHETYPE6_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2##_##x3##_##x4##_##x5##_##x6] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE7(x1, x2, x3, x4, x5, x6, x7) { .count = ARCHETYPE7_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2##_##x3##_##x4##_##x5##_##x6##_##x7] }
#define COMPONENT_SYSTEM_ACCESS_ARCHETYPE8(x1, x2, x3, x4, x5, x6, x7, x8) { .count = ARCHETYPE8_DEPENDENCIES_COUNT, .pointer = &ArchetypeDependencies[ARCHETYPE_DEPENDENCIES_##x1##_##x2##_##x3##_##x4##_##x5##_##x6##_##x7##_##x8] }

const CCString ComponentExpressionNames[ECS_COMPONENT_MAX] = {
    [ECS_COMPONENT_BASE_INDEX(COMP_A)] = CC_STRING("comp-a"),
    [ECS_COMPONENT_BASE_INDEX(COMP_B)] = CC_STRING("comp-b"),
    [ECS_COMPONENT_BASE_INDEX(COMP_C)] = CC_STRING("comp-c"),
    [ECS_COMPONENT_BASE_INDEX(COMP_D)] = CC_STRING("comp-d"),
    [ECS_COMPONENT_BASE_INDEX(COMP_E)] = CC_STRING("comp-e"),
    [ECS_COMPONENT_BASE_INDEX(ARCH_K)] = CC_STRING("arch-k"),
    [ECS_COMPONENT_BASE_INDEX(ARCH_H)] = CC_STRING("arch-h"),
    [ECS_COMPONENT_BASE_INDEX(ARCH_TAG)] = CC_STRING("arch-tag"),
    [ECS_COMPONENT_BASE_INDEX(COMP_F)] = CC_STRING("comp-f"),
    [ECS_COMPONENT_BASE_INDEX(ECS_MONITOR_COMPONENT)] = CC_STRING("monitor"),
    [ECS_COMPONENT_BASE_INDEX(COMP_J)] = CC_STRING("comp-j"),
    [ECS_COMPONENT_BASE_INDEX(PACKED_K)] = CC_STRING("packed-k"),
    [ECS_COMPONENT_BASE_INDEX(PACKED_H)] = CC_STRING("packed-h"),
    [ECS_COMPONENT_BASE_INDEX(CHECK_RUN_STATE_TAG)] = CC_STRING("check-run-state-tag"),
    [ECS_COMPONENT_BASE_INDEX(DESTROY_ME_TAG)] = CC_STRING("destroy-me-tag"),
    [ECS_COMPONENT_BASE_INDEX(DUPLICATE_A)] = CC_STRING("duplicate-a"),
    [ECS_COMPONENT_BASE_INDEX(PACKED_TAG)] = CC_STRING("packed-tag"),
    [ECS_COMPONENT_BASE_INDEX(COMP_G)] = CC_STRING("comp-g"),
    [ECS_COMPONENT_BASE_INDEX(COMP_H)] = CC_STRING("comp-h"),
    [ECS_COMPONENT_BASE_INDEX(COMP_I)] = CC_STRING("comp-i"),
    [ECS_COMPONENT_BASE_INDEX(ECS_NAME_COMPONENT)] = CC_STRING("name"),
    [ECS_COMPONENT_BASE_INDEX(INDEXED_K)] = CC_STRING("indexed-k"),
    [ECS_COMPONENT_BASE_INDEX(INDEXED_H)] = CC_STRING("indexed-h"),
    [ECS_COMPONENT_BASE_INDEX(INDEXED_TAG)] = CC_STRING("indexed-tag"),
    [ECS_COMPONENT_BASE_INDEX(LOCAL_K)] = CC_STRING("local-k"),
    [ECS_COMPONENT_BASE_INDEX(LOCAL_H)] = CC_STRING("local-h"),
    [ECS_COMPONENT_BASE_INDEX(LOCAL_DUPLICATE_B)] = CC_STRING("local-duplicate-b"),
    [ECS_COMPONENT_BASE_INDEX(LOCAL_TAG)] = CC_STRING("local-tag"),
};

const CCDictionary ComponentExpressionMaps[ECS_COMPONENT_MAX] = {
};

const ECSComponentID ComponentIDList[] = {
    COMP_F, COMP_G, COMP_H, 
    ARCH_TAG, COMP_A, 
    COMP_C, 
    COMP_A, COMP_I, 
    COMP_A, COMP_J, 
    LOCAL_DUPLICATE_B, LOCAL_H, 
    CHECK_RUN_STATE_TAG, 
    COMP_B, 
    COMP_D, 
    DESTROY_ME_TAG, 
    DUPLICATE_A, 
    ECS_MONITOR_COMPONENT, 
};

#define COMPONENT_ID_LIST_CompF_CompG_CompH (ComponentIDList + 0)
#define COMPONENT_ID_LIST_CompH (ComponentIDList + 2)
#define COMPONENT_ID_LIST_ArchTag_CompA (ComponentIDList + 3)
#define COMPONENT_ID_LIST_CompA (ComponentIDList + 4)
#define COMPONENT_ID_LIST_CompA_CompC (ComponentIDList + 4)
#define COMPONENT_ID_LIST_CompC (ComponentIDList + 5)
#define COMPONENT_ID_LIST_CompA_CompI (ComponentIDList + 6)
#define COMPONENT_ID_LIST_CompA_CompJ (ComponentIDList + 8)
#define COMPONENT_ID_LIST_CompJ (ComponentIDList + 9)
#define COMPONENT_ID_LIST_LocalDuplicateB_LocalH (ComponentIDList + 10)
#define COMPONENT_ID_LIST_CheckRunStateTag (ComponentIDList + 12)
#define COMPONENT_ID_LIST_CompB (ComponentIDList + 13)
#define COMPONENT_ID_LIST_CompD (ComponentIDList + 14)
#define COMPONENT_ID_LIST_DestroyMeTag (ComponentIDList + 15)
#define COMPONENT_ID_LIST_DuplicateA (ComponentIDList + 16)
#define COMPONENT_ID_LIST_ECSMonitorComponent (ComponentIDList + 17)

const size_t ComponentOffsetList[] = {
    offsetof(ECSContext, packed[(COMP_F & ~ECSComponentStorageMask)]), offsetof(ECSContext, packed[(COMP_G & ~ECSComponentStorageMask)]), offsetof(ECSContext, indexed[(COMP_H & ~ECSComponentStorageMask)]), offsetof(ECSContext, indexed[(COMP_I & ~ECSComponentStorageMask)]), 
    offsetof(ECSContext, packed[(CHECK_RUN_STATE_TAG & ~ECSComponentStorageMask)]), 
    offsetof(ECSContext, packed[(COMP_J & ~ECSComponentStorageMask)]), 
    offsetof(ECSContext, packed[(DESTROY_ME_TAG & ~ECSComponentStorageMask)]), 
    offsetof(ECSContext, packed[(DUPLICATE_A & ~ECSComponentStorageMask)]), 
    offsetof(ECSContext, packed[(ECS_MONITOR_COMPONENT & ~ECSComponentStorageMask)]), 
};

#define COMPONENT_OFFSET_LIST_CompF_CompG_CompH_CompI (ComponentOffsetList + 0)
#define COMPONENT_OFFSET_LIST_CompH (ComponentOffsetList + 2)
#define COMPONENT_OFFSET_LIST_CheckRunStateTag (ComponentOffsetList + 4)
#define COMPONENT_OFFSET_LIST_CompJ (ComponentOffsetList + 5)
#define COMPONENT_OFFSET_LIST_DestroyMeTag (ComponentOffsetList + 6)
#define COMPONENT_OFFSET_LIST_DuplicateA (ComponentOffsetList + 7)
#define COMPONENT_OFFSET_LIST_ECSMonitorComponent (ComponentOffsetList + 8)

#define TEST_GROUP 2
#define OTHER_GROUP 1
#define MISC_GROUP 0

const ECSGroupDependency GroupDependencies[] = {
#define MISC_GROUP_DEPENDENCIES (GroupDependencies + 0)
    { .group = -1, .priority = -1 },
    { .group = -1, .priority = -1 },
#define OTHER_GROUP_DEPENDENCIES (GroupDependencies + 2)
    { .group = -1, .priority = -1 },
#define TEST_GROUP_DEPENDENCIES (GroupDependencies + 3)
    { .group = MISC_GROUP, .priority = 1 },
    { .group = OTHER_GROUP, .priority = 0 },
    { .group = -1, .priority = -1 },
    { .group = -1, .priority = -1 },
};

const ECSSystemRange SystemRange[] = {
#define MISC_GROUP_SYSTEM_RANGE (SystemRange + 0)
    { 0, 9 },
    { 9, 5 },
#define OTHER_GROUP_SYSTEM_RANGE (SystemRange + 2)
    { 0, 1 },
#define TEST_GROUP_SYSTEM_RANGE (SystemRange + 3)
    { 0, 0 },
    { 0, 0 },
    { 0, 1 },
    { 1, 1 },
};

const ECSSystemUpdate SystemUpdate[] = {
#define MISC_GROUP_SYSTEM_UPDATE (SystemUpdate + 0)
    ECS_SYSTEM_UPDATE(Sys4ReadA),
    ECS_SYSTEM_UPDATE(Sys5ReadC),
    ECS_SYSTEM_UPDATE(Sys6ReadAC),
    ECS_SYSTEM_UPDATE_PARALLEL_CHUNK(Sys14ReadAJ, offsetof(ECSContext, packed[(COMP_J & ~ECSComponentStorageMask)].entities), SIZE_MAX),
    ECS_SYSTEM_UPDATE(Sys13ReadDestroyMeTag),
    ECS_SYSTEM_UPDATE(Sys7WriteB),
    ECS_SYSTEM_UPDATE(Sys1ReadA_WriteB),
    ECS_SYSTEM_UPDATE(Sys2ReadAC_WriteB),
    ECS_SYSTEM_UPDATE(Sys3ReadAC_WriteD),
    ECS_SYSTEM_UPDATE(Sys11ReadAWithArchTag),
    ECS_SYSTEM_UPDATE_PARALLEL_CHUNK(Sys15ReadH, offsetof(ECSContext, indexed[(COMP_H & ~ECSComponentStorageMask)]), SIZE_MAX),
    ECS_SYSTEM_UPDATE_PARALLEL(Sys8ReadD_WriteC),
    ECS_SYSTEM_UPDATE_PARALLEL_CHUNK(Sys10WriteJ, offsetof(ECSContext, packed[(COMP_J & ~ECSComponentStorageMask)].entities), SIZE_MAX),
    ECS_SYSTEM_UPDATE(Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA),
#define OTHER_GROUP_SYSTEM_UPDATE (SystemUpdate + 14)
    ECS_SYSTEM_UPDATE(Sys9ReadFGH_WriteAI),
#define TEST_GROUP_SYSTEM_UPDATE (SystemUpdate + 15)
    ECS_SYSTEM_UPDATE(Sys16ReadCheckRunStateTag),
    ECS_SYSTEM_UPDATE_PARALLEL_CHUNK(ECSMonitorSystem, offsetof(ECSContext, packed[(ECS_MONITOR_COMPONENT & ~ECSComponentStorageMask)].entities), ECS_MONITOR_SYSTEM_PARALLEL_CHUNK_SIZE),
};

const ECSSystemAccess SystemAccess[] = {
#define MISC_GROUP_SYSTEM_ACCESS (SystemAccess + 0)
    { .read = { .ids = COMPONENT_ID_LIST_CompA, .count = 1 }, .write = { .ids = NULL, .count = 0 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE1(CompA) },
    { .read = { .ids = COMPONENT_ID_LIST_CompC, .count = 1 }, .write = { .ids = NULL, .count = 0 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE1(CompC) },
    { .read = { .ids = COMPONENT_ID_LIST_CompA_CompC, .count = 2 }, .write = { .ids = NULL, .count = 0 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE2(CompA, CompC) },
    { .read = { .ids = COMPONENT_ID_LIST_CompA_CompJ, .count = 2 }, .write = { .ids = NULL, .count = 0 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE1(CompA), .component = { .offsets = COMPONENT_OFFSET_LIST_CompJ } },
    { .read = { .ids = COMPONENT_ID_LIST_DestroyMeTag, .count = 1 }, .write = { .ids = NULL, .count = 0 }, .component = { .offsets = COMPONENT_OFFSET_LIST_DestroyMeTag } },
    { .read = { .ids = NULL, .count = 0 }, .write = { .ids = COMPONENT_ID_LIST_CompB, .count = 1 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE1(CompB) },
    { .read = { .ids = COMPONENT_ID_LIST_CompA, .count = 1 }, .write = { .ids = COMPONENT_ID_LIST_CompB, .count = 1 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE2(CompA, CompB) },
    { .read = { .ids = COMPONENT_ID_LIST_CompA_CompC, .count = 2 }, .write = { .ids = COMPONENT_ID_LIST_CompB, .count = 1 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE3(CompA, CompB, CompC) },
    { .read = { .ids = COMPONENT_ID_LIST_CompA_CompC, .count = 2 }, .write = { .ids = COMPONENT_ID_LIST_CompD, .count = 1 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE3(CompA, CompC, CompD) },
    { .read = { .ids = COMPONENT_ID_LIST_ArchTag_CompA, .count = 2 }, .write = { .ids = NULL, .count = 0 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE2(ArchTag, CompA) },
    { .read = { .ids = COMPONENT_ID_LIST_CompH, .count = 1 }, .write = { .ids = NULL, .count = 0 }, .component = { .offsets = COMPONENT_OFFSET_LIST_CompH } },
    { .read = { .ids = COMPONENT_ID_LIST_CompD, .count = 1 }, .write = { .ids = COMPONENT_ID_LIST_CompC, .count = 1 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE2(CompC, CompD) },
    { .read = { .ids = NULL, .count = 0 }, .write = { .ids = COMPONENT_ID_LIST_CompJ, .count = 1 }, .component = { .offsets = COMPONENT_OFFSET_LIST_CompJ } },
    { .read = { .ids = COMPONENT_ID_LIST_LocalDuplicateB_LocalH, .count = 2 }, .write = { .ids = COMPONENT_ID_LIST_DuplicateA, .count = 1 }, .component = { .offsets = COMPONENT_OFFSET_LIST_DuplicateA } },
#define OTHER_GROUP_SYSTEM_ACCESS (SystemAccess + 14)
    { .read = { .ids = COMPONENT_ID_LIST_CompF_CompG_CompH, .count = 3 }, .write = { .ids = COMPONENT_ID_LIST_CompA_CompI, .count = 2 }, .archetype = COMPONENT_SYSTEM_ACCESS_ARCHETYPE1(CompA), .component = { .offsets = COMPONENT_OFFSET_LIST_CompF_CompG_CompH_CompI } },
#define TEST_GROUP_SYSTEM_ACCESS (SystemAccess + 15)
    { .read = { .ids = COMPONENT_ID_LIST_CheckRunStateTag, .count = 1 }, .write = { .ids = NULL, .count = 0 }, .component = { .offsets = COMPONENT_OFFSET_LIST_CheckRunStateTag } },
    { .read = { .ids = NULL, .count = 0 }, .write = { .ids = COMPONENT_ID_LIST_ECSMonitorComponent, .count = 1 }, .component = { .offsets = COMPONENT_OFFSET_LIST_ECSMonitorComponent } },
};

const uint8_t SystemGraph[] = {
#define MISC_GROUP_SYSTEM_GRAPH (SystemGraph + 0)
    254, 1, 253, 1, 251, 1, 247, 1, 239, 1, 31, 1, 31, 1, 31, 1, 255, 0, 
    30, 29, 27, 23, 15, 
#define OTHER_GROUP_SYSTEM_GRAPH (SystemGraph + 23)
    0, 
#define TEST_GROUP_SYSTEM_GRAPH (SystemGraph + 24)
    
    
    0, 
    0, 
};

const ECSGroup Groups[] = {
    {
        .freq = ECS_TIME_FROM_SECONDS(1.0 / 60.0),
        .dynamic = FALSE,
        .priorities = {
            .count = 2,
            .deps = MISC_GROUP_DEPENDENCIES,
            .systems = {
                .range = MISC_GROUP_SYSTEM_RANGE,
                .graphs = MISC_GROUP_SYSTEM_GRAPH,
                .update = MISC_GROUP_SYSTEM_UPDATE,
                .access = MISC_GROUP_SYSTEM_ACCESS,
            }
        }
    },
    {
        .freq = ECS_TIME_FROM_SECONDS(1.0 / 60.0) * 2,
        .dynamic = FALSE,
        .priorities = {
            .count = 1,
            .deps = OTHER_GROUP_DEPENDENCIES,
            .systems = {
                .range = OTHER_GROUP_SYSTEM_RANGE,
                .graphs = OTHER_GROUP_SYSTEM_GRAPH,
                .update = OTHER_GROUP_SYSTEM_UPDATE,
                .access = OTHER_GROUP_SYSTEM_ACCESS,
            }
        }
    },
    {
        .freq = ECS_TIME_FROM_SECONDS(1.0 / 60.0),
        .dynamic = FALSE,
        .priorities = {
            .count = 4,
            .deps = TEST_GROUP_DEPENDENCIES,
            .systems = {
                .range = TEST_GROUP_SYSTEM_RANGE,
                .graphs = TEST_GROUP_SYSTEM_GRAPH,
                .update = TEST_GROUP_SYSTEM_UPDATE,
                .access = TEST_GROUP_SYSTEM_ACCESS,
            }
        }
    },
};

