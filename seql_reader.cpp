////
//// Created by pnsv0 on 13.04.2022.
////
//
//#include "../include/seql_reader.hpp"
//
//void SEQL::Reader::loadFile(const std::string& file_dir) {
//    debug_log("Loading file at " + file_dir);
//    std::fstream file;
//    file.open(file_dir, std::fstream::in);
//
//    std::shared_ptr<Statement> mn = std::make_shared<Statement>();
//    this->group_stack.push(mn);
//
//    if(!file.is_open()){
//        std::cout << "Couldn't file at " << file_dir << std::endl;
//        return;
//    }
//
//    std::string buffer;
//    this->line = 0 ;
//    while(getline(file, buffer)) {
//        this->line++;
//        this->process_line(rtrim(ltrim(buffer)));
//    }
//    debug_log("END");
//}
//
//void SEQL::Reader::process_line(const std::string& line) {
//    Lexer lexer;
//    std::vector<SEQL::Token> fragments = lexer.tokenize(line);
//
//    bool add = true;
//    for (int i = 0; i < fragments.size(); ++i) {
//        auto element = fragments[i];
//
//        if(element.value == "}") {
//            this->group_stack.pop();
//            add = false;
//        }
//
//        if(element.type == SEQL::FragmentType::KEYWORD) {
//            std::vector<SEQL::Token> rest_of_fragments = std::vector<SEQL::Token>
//                    (fragments.begin() + i + 1, fragments.end());
//            this->process_keyword(element, rest_of_fragments, line);
//            if(element.value == "IF") {
//                add = false;
//                break;
//            }
//        }
//    }
//
//    if(add) {
//        auto st = std::make_shared<Statement>();
//        std::shared_ptr<Expression> exp = std::make_shared<Expression>();
//        exp->fragments = fragments;
//        st->exp = exp;
//        st->raw = line;
//        auto top = this->group_stack.top();
//        if(!top->lines.empty()) {
//            top->lines.back()->next = st;
//        }
//
//        top->lines.push_back(st);
//        debug_log("a");
//    }
//}
//
//void SEQL::Reader::process_keyword(SEQL::Token fragment, std::vector<SEQL::Token> next_fragments, const std::string& line_raw) {
//    if(fragment.value == "IF") {
//        std::vector<Token> condition_fragments;
//        bool reading_condition = true;
//
//        std::shared_ptr<SEQL::IfStatement> _if = std::make_shared<SEQL::IfStatement>();
//
//        for (int i = 0; i < next_fragments.size(); ++i) {
//            auto element = next_fragments[i];
//            if(i == 0) {
//                if (element.value != "(") {
//                    std::cout << "Expected ( sign after IF keyword";
//                    this->halt = true;
//                }
//                continue;
//            }
//
//            if(element.value == ")")
//                reading_condition = false;
//
//            if(reading_condition)
//                condition_fragments.push_back(element);
//
//            if(element.value == "{")
//                break;
//        }
//        _if->condition = condition_fragments;
//        _if->raw = line_raw;
//
//        append_link(_if);
//    }
//    if(fragment.value == "FOR") {
//        std::shared_ptr<SEQL::ForStatement> _for = std::make_shared<SEQL::ForStatement>();
//        std::vector<Expression> for_elements;
//
//        std::vector<Token> buffer;
//        if(next_fragments[0].value != "(") {
//
//            this->halt = true;
//        }
//        for (int i = 1; i < next_fragments.size(); ++i) {
//            auto element = next_fragments[i];
//            if(element.value == ";") {
//                Expression exp;
//                exp.fragments = buffer;
//                for_elements.push_back(exp);
//
//                buffer.clear();
//            }
//        }
//
//        if(for_elements.size() != 3) {
//            std::cout << "Invalid amount of for conditions at line " << this->line << std::endl;
//            std::cout << "-> " << line_raw << std::endl;
//            this->halt = true;
//            return;
//        }
//
//        _for->start = for_elements[0].fragments;
//        _for->condition = for_elements[1].fragments;
//        _for->each = for_elements[2].fragments;
//        _for->raw = line_raw;
//
//        append_link(_for);
//
//    }
//}
//
//void SEQL::Reader::append_link(const std::shared_ptr<Statement>& statement) {
//    auto stack_top = this->group_stack.top();
//    auto last = stack_top->lines.back();
//    last->next = statement;
//
//    stack_top->lines.push_back(statement);
//    this->group_stack.push(statement);
//}
//
//SEQL::Reader::Reader() {
//    std::shared_ptr<Statement> main = std::make_shared<Statement>();
//    this->group_stack.push(main);
//}
