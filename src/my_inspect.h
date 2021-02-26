//
// Created by aogun on 2021/1/18.
//

#ifndef MP4VIEWER_MY_INSPECT_H
#define MP4VIEWER_MY_INSPECT_H

#include "Ap4.h"
#include "atom_obj.h"
#include "mm_log.h"

class my_inspect : public AP4_AtomInspector {
public:
    explicit my_inspect();
    ~my_inspect() override;

    void AddFieldArray(AP4_Size column_num, AP4_Size row_num,
                       const char * column_names, AP4_UI64 *data) override;

    // methods
    void StartAtom(const char* name,
                   AP4_UI08    version,
                   AP4_UI32    flags,
                   AP4_Size    header_size,
                   AP4_UI64    size,
                   AP4_UI64    offset) override;
    void EndAtom() override;
    void StartDescriptor(const char* name,
                         AP4_Size    header_size,
                         AP4_UI64    size,
                         AP4_UI64    offset) override;
    void EndDescriptor() override;
    void AddField(const char* name, AP4_UI64 value, FormatHint hint) override;
    void AddFieldF(const char* name, float value, FormatHint hint) override;
    void AddField(const char* name, const char* value, FormatHint hint) override;
    void AddField(const char* name, const unsigned char* bytes, AP4_Size size, FormatHint hint) override;

    std::vector<std::shared_ptr<atom_obj>> * atoms() { return &m_atoms; }
private:
    std::vector<std::shared_ptr<atom_obj>> m_atoms;
    std::shared_ptr<atom_obj> m_current;
};

#endif //MP4VIEWER_MY_INSPECT_H
