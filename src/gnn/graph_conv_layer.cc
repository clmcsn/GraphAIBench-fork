#include "graph_conv_layer.h"
#include "cl_utils.h"

template <typename Aggregator>
graph_conv_layer<Aggregator>::graph_conv_layer(int id, int nv, int din, int dout, 
                 LearningGraph *g, bool act, bool concat, float lr, float feat_drop, float score_drop, std::string feats_drop_file /*= ""*/) :
    level_(id), num_samples(nv), dim_in(din), dim_out(dout), graph(g), is_act(act),
    is_bias(false), use_concat(concat), feat_dropout_rate(feat_drop), score_dropout_rate(score_drop), feats_drop_file(feats_drop_file) {

    auto x = num_samples;
    auto y = dim_in;
    auto z = dim_out;

    d_W_neigh = (float*) clMallocRW(y * z);
    auto init_range = sqrt(6.0 / (y + z));
    clInitRangeUniformMem(y + z, -init_range, init_range, d_W_neigh);

    d_in_temp = (float*) clMallocRW(x * y);
    d_out_temp = (float*) clMallocRW(x * z);
    clInitConstMem<float>(x * y, 0.0, d_in_temp);
    clInitConstMem<float>(x * z, 0.0, d_out_temp);
    if (y <= z){
        d_in_temp1 = (float*) clMallocRW(x * y);
        clInitConstMem<float>(x * y, 0.0, d_in_temp1);
    }
    if (level_ > 0) {
        feat_in = (float*) clMallocRW(x * y);
        clInitConstMem<float>(x * y, 0.0, feat_in);
    }

    if (is_bias) {
    d_bias = (float*) clMallocRW(z);
    clInitConstMem<float>(z, 0.0, d_bias);
    }
}

template <typename Aggregator>
void graph_conv_layer<Aggregator>::update_dim_size(size_t x) {
    if (x > num_samples) {
        auto y = dim_in;
        auto z = dim_out;
        d_in_temp = (float*) clReallocRW(d_in_temp, x * y);
        d_out_temp = (float*) clReallocRW(d_out_temp, x * z);
        clInitConstMem<float>(x * y, 0.0, d_in_temp);
        clInitConstMem<float>(x * z, 0.0, d_out_temp);
        if (y <= z){
            d_in_temp1 = (float*) clReallocRW(d_in_temp1, x * y);
            clInitConstMem<float>(x * y, 0.0, d_in_temp1);
        }
        if (level_ > 0) {
            feat_in = (float*) clReallocRW(feat_in, x * y);
            clInitConstMem<float>(x * y, 0.0, feat_in);
        }
    }
}

template <typename Aggregator>
void graph_conv_layer<Aggregator>::dump_feats_to_file(float *feats, int size) {
    float feats_drop[size];
    clMemcpyD2H( (cl_mem) feats, size * sizeof(float), &feats_drop[0]);
    std::ofstream ofs(feats_drop_file, std::ios::out | std::ios::binary);
    //output serialization
    for(int i = 0; i < size; i++) {
        ofs.write((char*)&feats_drop[i], sizeof(float));
    }
    ofs.close();
}

template class graph_conv_layer<GCN_Aggregator>;
template class graph_conv_layer<GAT_Aggregator>;
template class graph_conv_layer<SAGE_Aggregator>;